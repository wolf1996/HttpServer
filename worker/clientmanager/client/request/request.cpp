//
// Created by root on 07.10.17.
//

#include "request.h"
#include <iostream>
Request::Request(): curr_state(EMPTY), index(0){}

/*
 *    //auto len = additional_data.size();
 *    //curr_state = (additional_data[len-2] == '\r' && additional_data[len-1] == '\n')? FINISHED: METHOD;
 */

// здесь будет супер заглушка!
Request::state Request::process(std::string additional_data) {
    req += additional_data;

    for(; (index < req.size())&&((curr_state != FINISHED)||(curr_state != ERROR)); ){
        index = proc(index);

    }
    return  curr_state;
}

int Request::emptyState(int id){
    if (req[id] == 'G') {
        curr_state = METHOD;
        res.method = ParsedRequest::GET;
        return 0;
    }
    if (req[id] == 'H') {
        curr_state = METHOD;
        res.method = ParsedRequest::HEAD;
        return 0;
    }
    if (req[id] == 'P') {
        curr_state = HEADERS;
        res.method = ParsedRequest::POST;
        return 0;
    }
    curr_state = ERROR;
    return 0;
}
int Request::methodState(int id){
    for(;(id < req.size())&&(req[id] != ' '); id++){}
    if (req[id] != ' '){
        return id;
    }
    curr_state = PATH;
    return id + 1;
}
std::string urlDecode(std::string eString) {
    std::string ret;
    char ch;
    int i, j;
    for (i=0; i<eString.length(); i++) {
        if (int(eString[i])==37) {
            sscanf(eString.substr(i+1,2).c_str(), "%x", &j);
            ch=static_cast<char>(j);
            ret+=ch;
            i=i+2;
        } else {
            ret+=eString[i];
        }
    }
    return (ret);
}
int Request::pathState(int id){
    auto bg = id;
    for(;(id < req.size())&&(req[id] != ' ')&&(req[id] != '?'); id++){}
    res.path = urlDecode(std::string(req.begin() + bg, req.begin() + id));
    if (id == req.size()){
        return id;
    }
    curr_state = HTTP;
    return id + 1;
}
int Request::headersState(int id){
    auto ln = req.size();
    curr_state = FINISHED;
    return ln;
    /*if ((req[ln-1] == '\n') || (req[ln-2] == '\r')){
        curr_state = FINISHED;
        return ln;
    }
    return ln;*/
}

int Request::httpState(int id) {
    for(;(id < req.size())&&(req[id] != '\n'); id++){}
    if (id == req.size()){
        return id;
    }
    curr_state = HEADERS;
    return id + 1;
}

int Request::proc(int id) {
    switch (curr_state){
        case EMPTY:
            return emptyState(id);
        case METHOD:
            return methodState(id);
        case HEADERS:
            return headersState(id);
        case PATH:
            return pathState(id);
        case HTTP:
            return httpState(id);
    }
    return curr_state;
}

std::string Request::stringState(){
    switch (curr_state){
        case EMPTY:
            return std::string("empty");
        case METHOD:
            return std::string("method");
        case HEADERS:
            return std::string("headers");
        case PATH:
            return std::string("path");
        case HTTP:
            return std::string("http");
    }
    return "1";
}

std::string Request::getStringReq() {
    return req;
}

Request::ParsedRequest Request::getParsedReq(){
    return res;
}