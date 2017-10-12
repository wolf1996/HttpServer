//
// Created by root on 12.10.17.
//

#include "ResponceFiles.h"
#include <iostream>

ResponceFiles::ResponceFiles(int _socket): socket(_socket), position(0), current_state(IN_PROGRES){
}

void ResponceFiles::setfile(int _file, ssize_t _filesize) {
    file = _file;
    size = _filesize;
}

ResponceFiles::state ResponceFiles::process(){
    std::cerr<<"SENT prev "<<position<<std::endl;
    auto done  = sendfile(socket, file, &position, BLOCKSIZE);
    if (done <= 0){
        if(errno != EAGAIN) {
            current_state = ERROR;
            std::cerr << "ERROR SENT " << position << std::endl;
            return current_state;
        }
    }
    std::cerr<<"SENT "<<position<<" "<<size <<std::endl;
    if(position >= size){
        current_state = FINISH;
    }
    return current_state;
}