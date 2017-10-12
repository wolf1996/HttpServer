#include <iostream>
#include <experimental/filesystem>

#include "worker/worker.h"
#include <thread>
#include <netinet/in.h>
#include <cstring>
#include <vector>
#include <asm/ioctls.h>
#include <sys/ioctl.h>
#include "sys/socket.h"
#include "sys/types.h"
#include "unistd.h"


int main() {
    std::experimental::filesystem::path working_directory("/home/ksg/disk_d/Mail/HightLoad/http-test-suite/");
    int sock_descriptor;
    struct sockaddr_in addr;
    sock_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_descriptor < 0) {
        std::cerr << "can't create socket" << std::endl;
        return 0;
    }
    int on = 1;
    auto err = setsockopt(sock_descriptor, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on));
    if (err < 0)
    {
        std:: cerr << "setsockopt() failed";
        close(sock_descriptor);
        exit(-1);
    }
    err = ioctl(sock_descriptor, FIONBIO, (char *)&on);
    if (err < 0)
    {
        std::cerr << "ioctl() failed";
        close(sock_descriptor);
        exit(-1);
    }
    std::memset(& addr, 0,sizeof(addr) );
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(80);
    err = bind(sock_descriptor, (struct sockaddr *)&addr, sizeof(addr));
    if (err < 0) {
        std::cerr << "can't bind addres " << err << std::endl;
        return 0;
    }
    err = set_nonblocking(sock_descriptor);
    if (err < 0) {
        std::cerr << "can't set nonblocking mod " << err << std::endl;
        return 0;
    }
    listen(sock_descriptor, 50);
    unsigned cores = std::thread::hardware_concurrency();
    auto tpull = std::vector<std::thread>(cores);
    for (auto i = 0; i < cores; i++){
        std::cout<<i<<"_worker starts"<<std::endl;
        tpull[i] = std::thread(worker, i, sock_descriptor, working_directory);
        cpu_set_t cpu_set;
        CPU_ZERO(&cpu_set);
        CPU_SET(i, &cpu_set);
        int rc = pthread_setaffinity_np(tpull[i].native_handle(),
                                        sizeof(cpu_set_t), &cpu_set);
        if (rc != 0) {
            std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";
        };

    }
    for(auto &i : tpull){
        i.join();
    }
    close(sock_descriptor);
    return 0;
}