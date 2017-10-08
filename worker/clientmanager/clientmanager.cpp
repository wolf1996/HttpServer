//
// Created by root on 08.10.17.
//

#include "clientmanager.h"
template <typename socket >
ClientManager<socket>::ClientManager() {
}

template <typename socket >
int ClientManager<socket>::addClient(socket sc) {
    clients_list[sc] = Client(sc);
}

template <typename socket >
Client ClientManager<socket>::getClient(socket sc) {
    return clients_list[sc];
}