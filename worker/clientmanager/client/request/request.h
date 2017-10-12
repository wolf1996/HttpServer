//
// Created by root on 07.10.17.
//

#ifndef FST_REQUEST_H
#define FST_REQUEST_H

#include <string>

class Request {
public:
    struct ParsedRequest{
        enum Method{
            GET,
            HEAD,
            POST,
        };
        Method  method;
        std::string path;
    };
    enum state{
        EMPTY,
        METHOD,
        PATH,
        HTTP,
        HEADERS,
        FINISHED,
        ERROR,
    };
private:
    std::string req;
    state curr_state;
    ParsedRequest res;
    int emptyState(int);
    int methodState(int);
    int pathState(int);
    int headersState(int);
    int httpState(int);
    int proc(int);
    int index;
public:
    Request();
    state process(std::string);
    std::string getStringReq();
    std::string stringState();
    ParsedRequest getParsedReq();
};


#endif //FST_REQUEST_H
