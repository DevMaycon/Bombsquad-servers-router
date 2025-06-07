#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>
#include <unordered_map>
#include "includes/src/Core/Client.h"


class Server {
    private:
        int ServerID = 0;
        int MaxPlayers = 10;
        int LastClientID = 113;
        std::unordered_map<int, Client> Clients = {};

    public:
        Server(int ServerID);
        
        int AddClient(Client);
        int RemoveClient(Client);
        bool IsFull();
};

#endif // SERVER_H