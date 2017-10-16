//
// Created by root on 10.10.17.
//

#include "response.h"
#include <iostream>
#include <sys/socket.h>

Response::Response( int _socket,const std::experimental::filesystem::path& _working_directory): socket(_socket),
current_state(PREPARE), working_directory(_working_directory), current_header_position(0), resp_file(socket){
    str_resp.reserve(1024);
}

void  Response::setRequest(const Request::ParsedRequest &_req) {
    req = _req;
}

Response::state Response::process(){
    switch (current_state){
        // вот тут будет нестандартный переход, т.к. переход из prepare в HEADERS не требует
        // записи в сокет каких либо данных, крч, думаю, надо вынести это в отдельный метод, а не в state
        // Просто выпилить break (>_>) (<_<) (^_^)
        case PREPARE:
            current_state = prepareHandler();
        case HEADERS:
            current_state = headersHandler();
        case FILE:
            if (current_state == FILE) {
                current_state = fileHandler();
            }
        case CLOSE:
            if (current_state == CLOSE) {
                current_state = closeHandler();
            }

    }
    return current_state;
}


// Добавить проверку на поддиректорию используя абсолютный путь
Response::state Response::prepareHandler() {
    switch (req.method){
        case Request::ParsedRequest::POST :
            preparePostHandler();
            break;

        case Request::ParsedRequest::HEAD :
            prepareHeaderHandler();
            break;

        case Request::ParsedRequest::GET :
            prepareGetHandler();
            break;
    }
    headers.getStringRepr(stringmessage);
    //std::cerr << stringmessage;
    return HEADERS;
}

Response::state Response::closeHandler() {
    return FINISH;
}

Response::state Response::headersHandler() {
    auto datasize = stringmessage.length();
    current_header_position += send(socket, stringmessage.c_str()+current_header_position,(datasize-current_header_position),0);
    if (current_header_position >= datasize) {
        current_header_position = 0;
        if(req.method == Request::ParsedRequest::HEAD){
            return CLOSE;
        }
        return FILE;
    }
    return HEADERS;
}

Response::state Response::fileHandler() {
    auto status = resp_file.process();
    if (status == ResponceFiles::IN_PROGRES){
        return FILE;
    }
    if (status == ResponceFiles::ERROR){
        return ERROR;
    }
    std::fclose(filedescr);
    return CLOSE;
}

Response::state Response::preparePostHandler(){
    headers.code = ResponseHeaders::NOTIMPLEMENTED_405;
    return HEADERS;
}
Response::state Response::prepareHeaderHandler(){
    path = working_directory / std::experimental::filesystem::path(req.path);
    bool indexing;
    if (std::experimental::filesystem::is_directory(path)) {
        path.append("index.html");
        indexing = true;
    }

    if (!std::experimental::filesystem::exists(path)) {
        headers.code = (!indexing)?ResponseHeaders::NOTFOUND_404:ResponseHeaders::FORBIDDEN_403;
    } else {
        headers.code = ResponseHeaders::OK_200;
        headers.content_type = path.extension();
        headers.filesize = std::experimental::filesystem::file_size(path);
    }
    return HEADERS;
}

Response::state Response::prepareGetHandler(){
    std::error_code cd;
    path = std::experimental::filesystem::canonical( working_directory / std::experimental::filesystem::path(req.path),cd);
    if(path.string().compare(0, working_directory.string().size(), working_directory.string())){
        headers.code = ResponseHeaders::NOTFOUND_404;
        return HEADERS;
    }
    bool indexing = false;
    if (std::experimental::filesystem::is_directory(path)) {
        path.append("index.html");
        indexing = true;
    }
    filedescr = fopen(path.c_str(),"r");
    if (filedescr == NULL) {
        headers.code = (!indexing)?ResponseHeaders::NOTFOUND_404:ResponseHeaders::FORBIDDEN_403;
    } else {
        headers.code = ResponseHeaders::OK_200;
        headers.content_type = path.extension();
        headers.filesize = std::experimental::filesystem::file_size(path);
        resp_file.setfile(fileno(filedescr),headers.filesize);
    }
    return HEADERS;
}