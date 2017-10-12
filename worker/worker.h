//
// Created by ksg on 05.10.17.
//

#ifndef FST_WORKER_H
#define FST_WORKER_H

#include <experimental/filesystem>
#include <iosfwd>

int set_nonblocking(int socket);
void worker(int worker_id, int socket, const std::experimental::filesystem::path& path);

#endif //FST_WORKER_H
