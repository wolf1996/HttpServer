//
// Created by root on 08.10.17.
//

#ifndef FST_CLIENTMANAGER_H
#define FST_CLIENTMANAGER_H

#include <map>
#include "client/client.h"
#include "iostream"
#define BUFSIZE 1024

template <typename sock>
class ClientManager {
    std::map<sock,Client*> clients_list;
    char buf[BUFSIZE];
    const std::experimental::filesystem::path& working_directory;
public:
    ClientManager(const std::experimental::filesystem::path& _working_directory);
    int addClient(sock);
    Client* getClient(sock);
    int removeClient(sock);
};

template <typename socket >
ClientManager<socket>::ClientManager(const std::experimental::filesystem::path& _working_directory):
working_directory(_working_directory){
}

template <typename socket >
int ClientManager<socket>::addClient(socket sc) {
    clients_list.insert(std::pair<socket, Client*>(sc, new Client(sc, buf, BUFSIZE, working_directory)));
    return 0;
}

template <typename socket >
int ClientManager<socket>::removeClient(socket sc) {
    auto cli = clients_list.find(sc)->second;
    //std::cerr<< cli->getEventsNum() << std::endl;
    delete cli;
    clients_list.erase(sc);
    return 0;

}

template <typename socket >
Client* ClientManager<socket>::getClient(socket sc) {
    return clients_list.find(sc)->second;
}

#endif //FST_CLIENTMANAGER_H
