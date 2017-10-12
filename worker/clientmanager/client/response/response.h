//
// Created by root on 10.10.17.
//

#ifndef FST_RESPONSE_H
#define FST_RESPONSE_H

#include "../request/request.h"
#include "ResponseHeaders/response_obj.h"
#include "ResponseFiles/ResponceFiles.h"
#include <experimental/filesystem>


class Response {
public:
    enum state{
        PREPARE,
        HEADERS,
        FILE,
        ERROR,
        CLOSE,
        FINISH
    };
private:
    Request::ParsedRequest req;
    int socket;
    std::FILE* filedescr;
    uint64_t current_header_position;
    ResponceFiles resp_file;
    std::string stringmessage;
    state current_state;
    state prepareHandler();
    state headersHandler();
    state closeHandler();
    state fileHandler();
    state preparePostHandler();
    state prepareHeaderHandler();
    state prepareGetHandler();
    ResponseHeaders headers;
    std::experimental::filesystem::path path;
    const std::experimental::filesystem::path& working_directory;
public:

    Response( int _socket, const std::experimental::filesystem::path& _working_directory);
    state process();
    void setRequest(const Request::ParsedRequest& req);

};


#endif //FST_RESPONSE_H
