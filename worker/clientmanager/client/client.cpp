//
// Created by ksg on 07.10.17.
//

#include <iostream>
#include <sys/epoll.h>
#include <tuple>
#include "client.h"


Client::Client(int _socket, char _buffer[], int _buffersize, const std::experimental::filesystem::path& _working_directory):
        socket(_socket), buffer(_buffer),buffersize(_buffersize),current_state(REQUEST_PARSING),
        working_directory(_working_directory), resp(_socket, _working_directory){}

Client::state Client::handle(uint32_t event) {
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


std::tuple<std::string, int> Client::readData() {
    ssize_t rd;
    std::string res;
    while(1){
        rd = read(socket, buffer, buffersize);
        if(rd <= 0) {
            if (errno != EAGAIN)
            {
                return std::make_tuple(std::string(""),1);
            }
            break;
        }
        res += std::string(buffer, buffer+rd);
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
    if (error != 0) {
        return ERROR;
    }
    auto st = req.process(data);
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

