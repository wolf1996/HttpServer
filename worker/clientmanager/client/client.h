//
// Created by ksg on 07.10.17.
//

#ifndef FST_CLIENT_H
#define FST_CLIENT_H

#include <string>
#include "request/request.h"

class Client {
    int socket;
    Request req;
public:
    enum out_state{
        OK,
        ERROR,
        FINISHED,
    };
    Client(int _socket);
    out_state handle(std::string);
    std::string getFullReq();
};


#endif //FST_CLIENT_H