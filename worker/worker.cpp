//
// Created by ksg on 05.10.17.
//

#include "worker.h"
#include "fstream"
#include "sys/socket.h"
#include <unistd.h>
#include <sys/epoll.h>
#include <cstring>
#include <asm/ioctls.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#define BUFSIZE 1024
#define EVENTLIST_SIZE 256
#define TIMEOUT 3000

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

void worker(int worker_id, int socket){
    int current_sockets = 1;
    std::ofstream logfile;
    epoll_event filter;
    epoll_event *event_list;
    logfile.open(std::to_string(worker_id));
    logfile << "worker starts" << std::endl;
    auto pollfd = epoll_create1(0);
    filter.data.fd = socket;
    filter.events = EPOLLIN | EPOLLET;
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
                (!(event_list[i].events & EPOLLIN)))
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
                    logfile << "Add new connection \n"<<std::endl;
                }while(!stop);
                continue;
            }
            auto socket_to_close = false;
            while(1){
                auto rd = read(current_socket, buffer, BUFSIZE);
                if(rd < 0) {
                    if (errno != EAGAIN)
                    {
                        socket_to_close = true;
                    }
                    break;
                }
                if (rd == 0){
                    socket_to_close = true;
                    break;
                }
                logfile << std::string(buffer, buffer+rd) << std::endl;
                if((buffer[rd-2]=='\r') && (buffer[rd-1]=='\n')){
                    socket_to_close = true;
                }
            }
            if(socket_to_close) {
                logfile << "close socket" << std::endl;
                close(current_socket);
            }
        }

    } while(!stop_this_nonsense);
    logfile << "worker stops";
    logfile.close();
}
