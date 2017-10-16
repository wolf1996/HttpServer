#include <iostream>
#include <experimental/filesystem>

#include "worker/worker.h"
#include <thread>
#include <netinet/in.h>
#include <cstring>
#include <fstream>
#include <vector>
#include <asm/ioctls.h>
#include <sys/ioctl.h>
#include "sys/socket.h"
#include "sys/types.h"
#include "unistd.h"
#include <regex>
#include <string>

// #define CONFIGFILEPATH "/home/ksg/disk_d/Mail/HightLoad/fst/httpd.conf"
#define CONFIGFILEPATH "/etc/httpd.conf"


struct server_config{
    int port;
    std::experimental::filesystem::path working_dir;
    int thread_limit;
    int cpu_limit;
    server_config():port(80),working_dir("."),thread_limit(50),cpu_limit(std::thread::hardware_concurrency()) {};
};

int parse_config(server_config& conf, std::string filename){
    std::string line;
    std::ifstream file;
    file.open(filename.c_str());
    std::smatch matches;
    if (file.fail()){
        std::cerr << "can't open config file " << std::endl;
        return 1;
    }
    std::regex conflineparse("^([A-Za-z0-9_]+)( *)([A-Za-z0-9_/-]+)( *)(.*)$", std::regex_constants::ECMAScript);
    while (std::getline(file,line)){
        if(std::regex_search(line, matches, conflineparse)) {
            auto key = matches[1].str();
            auto val = matches[3].str();
            std::cerr <<"KEY "<< key <<"\t VALUE "<<val << std::endl;
            if (!key.compare("listen")){
                conf.port = std::stoi(val, nullptr);
                continue;
            }
            if (!key.compare("cpu_limit")){
                conf.cpu_limit = std::stoi(val, nullptr);
                continue;
            }
            if (!key.compare("thread_limit")){
                conf.thread_limit = std::stoi(val, nullptr);
                continue;
            }
            if (!key.compare("document_root")){
                conf.working_dir = std::experimental::filesystem::path(val);
                continue;
            }
            std::cerr <<"ERROR"<< std::endl;
        } else {
            if(line.length() > 0) {
                std::cout << "conf parsing error" << line << std::endl;
            }
        }
    }
    return 0;
}

int main() {
    int sock_descriptor;
    struct sockaddr_in addr;
    server_config conf;
    if (parse_config(conf, CONFIGFILEPATH)){
        return 0;
    }
    std::experimental::filesystem::path working_directory = std::experimental::filesystem::canonical(conf.working_dir);
    std::cerr << working_directory << std::endl;
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
    addr.sin_port = htons(conf.port);
    auto num_cpu = std::thread::hardware_concurrency();
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
    listen(sock_descriptor, conf.thread_limit);
    unsigned cores = conf.cpu_limit;
    auto tpull = std::vector<std::thread>(cores);
    for (auto i = 0; i < cores; i++){
        std::cout<<i<<"_worker starts"<<std::endl;
        tpull[i] = std::thread(worker, i, sock_descriptor, working_directory);
        cpu_set_t cpu_set;
        CPU_ZERO(&cpu_set);
        CPU_SET(i%num_cpu, &cpu_set);
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