//
// Created by ksg on 07.10.17.
//

#include <iostream>
#include <sys/epoll.h>
#include <tuple>
#include "client.h"


Client::Client(int _socket, char _buffer[], int _buffersize):
        socket(_socket), buffer(_buffer),buffersize(_buffersize),current_state(REQUEST_PARSING){}

Client::state Client::handle(uint32_t event) {
    if((current_state == REQUEST_PARSING)&&(event & EPOLLIN)){
        return readHandler();
    }
    if ((current_state == RESPONCE_PROCESSING)&&(event & EPOLLOUT)){
        return writeHandler();
    }
}


std::tuple<std::string, int> Client::readData() {
    ssize_t rd;
    std::string res;
    while(1){
        rd = read(socket, buffer, buffersize);
        if(rd < 0) {
            if (errno != EAGAIN)
            {
                return std::make_tuple(std::string(""),1);
            }
            break;
        }
        if (rd == 0){
            return std::make_tuple(std::string(""),1);
        }
        res += std::string(buffer, buffer+rd);
        //logfile << std::string(buffer, buffer+rd) << std::endl;
    }
    return std::make_tuple(res,0);
}

std::string Client::getFullReq() {
    return req.getStringReq();
}

Client::state Client::readHandler(){
    std::string data;
    int error;
    std::tie(data, error) = readData();
    auto st = req.process(data);
    if (st == Request::FINISHED){
        return FINISHED;
    }
    if (st == Request::ERROR){
        return ERROR;
    }
    return OK;
}

Client::state Client::writeHandler(){
    return OK;
}