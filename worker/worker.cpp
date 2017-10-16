//
// Created by ksg on 05.10.17.
//

#include "worker.h"
#include "fstream"
#include "sys/socket.h"
#include "clientmanager/clientmanager.h"
#include <unistd.h>
#include <sys/epoll.h>
#include <cstring>
#include <fcntl.h>

#define BUFSIZE 1024
#define EVENTLIST_SIZE 1024
#define TIMEOUT -1

int set_nonblocking(int socket){
    int flags, s;

    flags = fcntl (socket, F_GETFL, 0);
    if (flags == -1)
    {
        return -1;
    }

    flags |= O_NONBLOCK;
    s = fcntl (socket, F_SETFL, flags);
    if (s == -1)
    {
        return -1;
    }

    return 0;
}

void worker(int worker_id, int socket, const std::experimental::filesystem::path& working_directory){
    int current_sockets = 1;
    std::ofstream logfile;
    epoll_event filter;
    epoll_event *event_list;
    logfile.open(std::to_string(worker_id));
    logfile << "worker starts" << std::endl;
    ClientManager<decltype(socket)> clients(working_directory);
    auto pollfd = epoll_create1(0);
    filter.data.fd = socket;
    filter.events = EPOLLIN | EPOLLOUT | EPOLLET;
    auto err = epoll_ctl(pollfd,EPOLL_CTL_ADD,socket, &filter);
    if (err < 0) {
        logfile << "error on epoll comand" << std::endl;
        return;
    }
    auto stop_this_nonsense = false;
    event_list = (epoll_event*)calloc(EVENTLIST_SIZE, sizeof(filter));
    char buffer[BUFSIZE];
    do{
        auto eventsnum = epoll_wait(pollfd, event_list, EVENTLIST_SIZE, TIMEOUT);
        if(eventsnum < 0){
            logfile << "polling error " << std::endl;
            break;
        }
        for(int i = 0; i < eventsnum; i++){
            if ((event_list[i].events & EPOLLERR) ||
                (event_list[i].events & EPOLLHUP) ||
                (!(event_list[i].events & (EPOLLIN|EPOLLOUT))))
            {
                logfile << "epoll error \n"<<std::endl;
                close (event_list[i].data.fd);
                continue;
            }
            auto current_event = event_list[i];
            auto current_socket = current_event.data.fd;
            if (socket == current_socket){
                bool stop = false;
                do{
                    auto client_con = accept(socket, NULL, NULL);
                    if(client_con < 0) {
                        if ((errno == EAGAIN) ||
                            (errno == EWOULDBLOCK))
                        {
                            stop = true;
                            break;
                        } else {
                            break;
                        }
                    }
                    err = set_nonblocking(client_con);
                    if (err < 0) {
                        stop_this_nonsense = true;
                        stop = true;
                        break;
                    }
                    filter.data.fd = client_con;
                    filter.events = EPOLLIN | EPOLLET;
                    err = epoll_ctl (pollfd, EPOLL_CTL_ADD, client_con, &filter);
                    if (err == -1)
                    {
                        stop_this_nonsense = true;
                        stop = true;
                        break;
                    }
                    clients.addClient(client_con);
                    //logfile << "Add new connection \n"<<std::endl;
                }while(!stop);
                continue;
            }
            auto socket_to_close = false;
            auto client = clients.getClient(current_socket);
            auto cs = client->getCurrState();
            auto res = client->handle(event_list[i].events);
            if((res == Client::FINISHED)||(res == Client::ERROR)){
                //logfile << client->getFullReq() << std::endl;
                socket_to_close = true;
            }
            if((!socket_to_close)&&((cs != res )&&(res == Client::RESPONCE_PROCESSING))){
                filter.data.fd = current_socket;
                filter.events = EPOLLOUT;
                err = epoll_ctl (pollfd, EPOLL_CTL_MOD, current_socket, &filter);
            }
            if (err == -1)
            {
                stop_this_nonsense = true;
                break;
            }
            if(socket_to_close) {
                //logfile << "close socket" << std::endl;
                clients.removeClient(current_socket);
                err = epoll_ctl(pollfd, EPOLL_CTL_DEL, current_socket, &filter);
                if (err == -1)
                {
                    stop_this_nonsense = true;
                    break;
                }
                close(current_socket);
            }
        }

    } while(!stop_this_nonsense);
    //logfile << "worker stops";
    logfile.close();
}
