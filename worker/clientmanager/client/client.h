//
// Created by ksg on 07.10.17.
//

#ifndef FST_CLIENT_H
#define FST_CLIENT_H

#include <string>
#include "request/request.h"
#include "response/response.h"
#include <asm/ioctls.h>
#include <sys/ioctl.h>
#include "sys/socket.h"
#include <unistd.h>
#include <sys/epoll.h>
#include <cstring>
#include <fcntl.h>
#include <experimental/filesystem>

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
    Response resp;
    state current_state;
    int readData();
    state readHandler();
    state writeHandler();
    char* buffer;
    int buffersize;
    const std::experimental::filesystem::path& working_directory;
    int eventsNum;
    std::string resp_string;
    std::string req_string;
public:
    Client(int _socket, char _buffer[], int _buffersize, const std::experimental::filesystem::path& _working_directory);
    state handle(uint32_t event);
    std::string getFullReq();
    int getEventsNum(){
        return  eventsNum;
    };
    state getCurrState(){
        return current_state ;
    }
};


#endif //FST_CLIENT_H
