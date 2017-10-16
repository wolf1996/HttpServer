//
// Created by root on 10.10.17.
//

#ifndef FST_RESPONSE_OBJ_H
#define FST_RESPONSE_OBJ_H

#include <string>
#include <sstream>

struct ResponseHeaders {
    enum code {
        OK_200,
        NOTFOUND_404,
        NOTIMPLEMENTED_405,
        FORBIDDEN_403,
    };
    bool body;
    std::string content_type;
    code code;
    uint64_t filesize;
    void getStringStatus(std::string &);
    void getContentType(std::string &);
    void getStaticHeaders(std::string &);
    void getStringRepr(std::string &);
    void getContentLength(std::string &);
};


#endif //FST_RESPONSE_OBJ_H
