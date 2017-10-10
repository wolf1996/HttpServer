//
// Created by ksg on 07.10.17.
//

#ifndef FST_CLIENT_H
#define FST_CLIENT_H

#include <string>
#include "request/request.h"
#include <asm/ioctls.h>
#include <sys/ioctl.h>
#include "sys/socket.h"
#include <unistd.h>
#include <sys/epoll.h>
#include <cstring>
#include <fcntl.h>

class Client {
public:
    enum state{
        OK,
        ERROR,
        REQUEST_PARSING,
        RESPONCE_PROCESSING,
        FINISHED,
    };
private:
    int socket;
    Request req;
    state current_state;
    std::tuple<std::string, int>readData();
    state readHandler();
    state writeHandler();
    char* buffer;
    int buffersize;
public:
    Client(int _socket, char _buffer[], int _buffersize);
    state handle(uint32_t event);
    std::string getFullReq();
};


#endif //FST_CLIENT_H
