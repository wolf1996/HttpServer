//
// Created by root on 08.10.17.
//

#ifndef FST_CLIENTMANAGER_H
#define FST_CLIENTMANAGER_H

#include <map>
#include "client/client.h"

template <typename sock>
class ClientManager {
    std::map<sock,Client> clients_list;
public:
    ClientManager();
    int addClient(sock);
    Client getClient(sock);
};


#endif //FST_CLIENTMANAGER_H
