//
// Created by root on 10.10.17.
//

#include "response_obj.h"
#include <map>
#include <ctime>
#include <iomanip>


void ResponseHeaders::getStringStatus(std::string & rpr){
    switch (code){
        case OK_200:
            rpr.append("HTTP/1.1 200 OK\r\n");
            break;
        case NOTFOUND_404:
            rpr.append("HTTP/1.1 404 Not Found\r\n");
            break;
        case NOTIMPLEMENTED_405:
            rpr.append("HTTP/1.1 405 Not Implemented\r\n");
            break;
        case FORBIDDEN_403:
            rpr.append("HTTP/1.1 403 FORBIDDEN\r\n");
            break;
        default:
            rpr.append("DICH \r\n");
    }
}

void ResponseHeaders::getContentType(std::string & rpr){
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
    rpr.append( table[content_type]);
}

void ResponseHeaders::getContentLength(std::string & rpr){
    rpr.append("Content-Length: " );
    rpr.append(std::to_string(filesize));
    rpr.append("\r\n");
}
void ResponseHeaders::getStaticHeaders(std::string & rpr){
    char buf[1000];
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    rpr.append("Server: HAL9k\r\nDate:");
    strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);
    rpr.append(buf);
    rpr.append("\r\nConnection: Closed\r\n\r\n");
}

void ResponseHeaders::getStringRepr(std::string& str){
    getStringStatus(str);
    if (code == OK_200){
        getContentType(str);
        getContentLength(str);
    }
    getStaticHeaders(str);
}