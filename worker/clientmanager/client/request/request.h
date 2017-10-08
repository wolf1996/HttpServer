//
// Created by root on 07.10.17.
//

#ifndef FST_REQUEST_H
#define FST_REQUEST_H

#include <string>

class Request {
    std::string req;
    state state;
public:
    enum state{
        METHOD,
        HEADERS,
        FINISHED,
        ERROR,
    };
    struct ParsedRequest{
        enum Method{
            GET,
            HEAD,
        };
    };
    Request();
    state process(std::string);
    std::string getStringReq();
};


#endif //FST_REQUEST_H
