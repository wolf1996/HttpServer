//
// Created by ksg on 07.10.17.
//

#include <iostream>
#include <sys/epoll.h>
#include <tuple>
#include "client.h"

Client::Client(int _socket, char _buffer[], int _buffersize, const std::experimental::filesystem::path& _working_directory):
        socket(_socket), buffer(_buffer),buffersize(_buffersize),current_state(REQUEST_PARSING),
        working_directory(_working_directory), resp(_socket, _working_directory),eventsNum(0){
    resp_string.reserve(1024);
    req_string.reserve(1024);
}

Client::state Client::handle(uint32_t event) {
    eventsNum++;
    // Короче, тут прикол EPOLL не перепосылает event-ы поэтому если делать свитч мы потеряем
    // event на write, т.к. мы его примем, но не обработаем.
    // RIP 2 часа моего времени.
    if (REQUEST_PARSING == current_state) {
        if (event & EPOLLIN) {
            current_state = readHandler();
        }
    }
    if (RESPONCE_PROCESSING == current_state){

        if (event & EPOLLOUT) {
            current_state = writeHandler();
        }
    }
    return current_state;
}


int Client::readData() {
    ssize_t app = 0;
    int seria = 0;
    while(1){
        app = read(socket, buffer, buffersize);
        if(app <= 0) {
            if (errno != EAGAIN)
            {
                return -1;
            }
            break;
        }
        req_string.append(buffer,buffer+app);
        seria += app;
    }
    return seria;
}

std::string Client::getFullReq() {
    return req.getStringReq();
}

Client::state Client::readHandler(){
    int num;
    num = readData();
    //std::cerr <<"GOT"<< data << std::endl;
    if (num < 0) {
        return ERROR;
    }
    if (num == 0){
        return RESPONCE_PROCESSING;
    }
    auto st = req.process(req_string);
    if (st == Request::FINISHED){
        resp.setRequest(req.getParsedReq());
        return RESPONCE_PROCESSING;
    }
    if (st == Request::ERROR){
        return ERROR;
    }
    return REQUEST_PARSING;
}

Client::state Client::writeHandler(){
    Response::state st = resp.process();
    if (st == Response::ERROR){
        return ERROR;
    }
    if (st == Response::FINISH){
        return FINISHED;
    }
    return RESPONCE_PROCESSING;
}

