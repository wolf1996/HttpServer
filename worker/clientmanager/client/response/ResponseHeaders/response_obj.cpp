//
// Created by root on 10.10.17.
//

#include "response_obj.h"
#include <map>

std::string ResponseHeaders::getStringStatus(){
    switch (code){
        case OK_200:
            return "HTTP/1.1 200 OK\r\n";
        case NOTFOUND_404:
            return "HTTP/1.1 404 Not Found\r\n";
        case NOTIMPLEMENTED_405:
            return "HTTP/1.1 405 Not Implemented\r\n";
        case FORBIDDEN_403:
            return "HTTP/1.1 403 FORBIDDEN\r\n";
    }
    return std::string("DICH \r\n");
}
std::string ResponseHeaders::getContentType(){
    static std::map<std::string, std::string> table{
            {".PNG","Content-Type: image/png\r\n"},
            {".png","Content-Type: image/png\r\n"},
            {".swf","Content-Type: application/x-shockwave-flash\r\n"},
            {".JPG","Content-Type: image/jpeg\r\n"},
            {".gif","Content-Type: image/gif\r\n"},
            {".css","Content-Type: text/css\r\n"},
            {".js","Content-Type: text/javascript\r\n"},
            {".jpg","Content-Type: image/jpeg\r\n"},
            {".txt","Content-Type: text/plain\r\n"},
            {".html","Content-Type: text/html\r\n"},
            {".jpeg","Content-Type: image/jpeg\r\n"},
    };
    return table[content_type];
}

std::string ResponseHeaders::getContentLength(){
    std::ostringstream oss;
    oss << "Content-Length: " << filesize << "\r\n";
    return oss.str();
}
std::string ResponseHeaders::getStaticHeaders(){
    return "Server: HAL9k\r\nDate Mon, 27 Jul 2009 12:28:53 GMT\r\nConnection: Closed\r\n\r\n";
}
std::string ResponseHeaders::getStringRepr(){
    std::string res = getStringStatus();
    if (code == OK_200){
        res += getContentType();
        res += getContentLength();
    }
    res += getStaticHeaders();
    return res;
}