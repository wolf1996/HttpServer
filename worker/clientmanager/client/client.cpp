//
// Created by ksg on 07.10.17.
//

#include <iostream>
#include "client.h"


Client::Client(int _socket): socket(_socket){}

Client::out_state Client::handle(std::string data) {
    auto st = req.process(data);
    if (st == Request::FINISHED){
        return FINISHED;
    }
    if (st == Request::ERROR){
        return ERROR;
    }
    return OK;
}

std::string Client::getFullReq() {
    return req.getStringReq();
}