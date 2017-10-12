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
    };
    bool body;
    std::string content_type;
    code code;
    uint64_t filesize;
    static const std::string static_header;
    std::string getStringStatus();
    std::string getContentType();
    std::string getStaticHeaders();
    std::string getStringRepr();
    std::string getContentLength();
};


#endif //FST_RESPONSE_OBJ_H
