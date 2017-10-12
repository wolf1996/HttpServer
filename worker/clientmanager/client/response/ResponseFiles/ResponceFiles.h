//
// Created by root on 12.10.17.
//

#ifndef FST_RESPONCEFILES_H
#define FST_RESPONCEFILES_H

#include <experimental/filesystem>
#include <sys/sendfile.h>

#define BLOCKSIZE 512000

class ResponceFiles {
public:
    enum state{
        ERROR,
        FINISH,
        IN_PROGRES,
    };
private:
    state current_state;
    ssize_t position;
    ssize_t size;
    int file;
    int socket;
public:

    ResponceFiles( int _socket);
    void setfile(int _file, ssize_t _filesize);
    state process();

};


#endif //FST_RESPONCEFILES_H
