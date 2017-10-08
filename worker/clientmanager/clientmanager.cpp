//
// Created by root on 08.10.17.
//

#include "clientmanager.h"
#include <client/client.h>

ClientManager::ClientManager() {
}

template <typename socket >
int ClientManager::addClient(socket sc) {
    clients_list[sc] = Client(sc);
}

template <typename socket >
Client ClientManager::getClient(socket sc) {
    return clients_list[sc];
}