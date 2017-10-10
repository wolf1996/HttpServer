//
// Created by root on 08.10.17.
//

#ifndef FST_CLIENTMANAGER_H
#define FST_CLIENTMANAGER_H

#include <map>
#include "client/client.h"
#define BUFSIZE 1024

template <typename sock>
class ClientManager {
    std::map<sock,Client> clients_list;
    char buf[BUFSIZE];
public:
    ClientManager();
    int addClient(sock);
    Client getClient(sock);
    int removeClient(sock);
};

template <typename socket >
ClientManager<socket>::ClientManager() {
}

template <typename socket >
int ClientManager<socket>::addClient(socket sc) {
    clients_list.insert(std::pair<socket, Client>(sc, Client(sc,buf, BUFSIZE)));
}

template <typename socket >
int ClientManager<socket>::removeClient(socket sc) {
    clients_list.erase(sc);
}

template <typename socket >
Client ClientManager<socket>::getClient(socket sc) {
    return clients_list.find(sc)->second;
}

#endif //FST_CLIENTMANAGER_H
