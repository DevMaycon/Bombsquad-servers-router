#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>
#include <unordered_map>
#include "includes/src/Core/Client.h"


class Server {
    private:
        sockaddr_in ServerAddress;
        std::unordered_map<int, Client> Clients = {};

        int ServerID = 0;
        int MaxPlayers = 10;
        int LastClientID = 113;

    public:
        Server(int ServerID, sockaddr_in ServerAddress);

        std::unordered_map<int, Client> GetClients();
        sockaddr_in GetAddress();
        int GetServerID();


        int AddressIsConnected(sockaddr_in address);
        int AddClient(sockaddr_in Address);
        int RemoveClient(sockaddr_in Address);

        bool IsFull();
};

#endif // SERVER_H
