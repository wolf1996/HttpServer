//
// Created by root on 07.10.17.
//

#include "request.h"
Request::Request(): curr_state(METHOD){}

Request::state Request::process(std::string additional_data) {
    req += additional_data;
    auto len = additional_data.size();
    curr_state = (additional_data[len-1] == '\n' && additional_data[len-1] == '\n')? METHOD: FINISHED;
    return  curr_state;
}

std::string Request::getStringReq() {
    return req;
}