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
    auto done  = sendfile(socket, file, &position, BLOCKSIZE);
    if (done <= 0){
        if(errno != EAGAIN) {
            current_state = ERROR;
            return current_state;
        }
    }
    if(position >= size){
        current_state = FINISH;
    }
    return current_state;
}