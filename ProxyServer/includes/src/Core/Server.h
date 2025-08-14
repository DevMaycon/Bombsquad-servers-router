#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>
#include <unordered_map>
#include <queue>
#include "includes/src/Core/Client.h"

struct ClientJoinRequest {
    sockaddr_in address;
    uint8_t request_id;
};

class Server {
    private:
        sockaddr_in ServerAddress;
        std::unordered_map<uint8_t, ClientJoinRequest> ClientsJoinQueue = {};
        std::unordered_map<uint8_t, Client> Clients = {};

        int ServerID = 0;
        int MaxPlayers = 10;

    public:
        Server() {};
        Server(const int ServerID, sockaddr_in ServerAddress) : ServerID(ServerID), ServerAddress(ServerAddress) {};

        std::unordered_map<uint8_t, ClientJoinRequest> GetClientsQueue();
        std::unordered_map<uint8_t, Client> &GetClients();
        int GetServerID();


        sockaddr_in GetAddress();
        bool AddressIsConnected(sockaddr_in Address);

        int AddClient(uint8_t ClientID, sockaddr_in ClientAddress);
        int ClientRequest(ClientJoinRequest JoinRequest);
        int ClientJoin(uint8_t ClientID, uint8_t RequestID);
        int ClientExit(uint8_t ClientID);

        bool IsFull();
};

#endif // SERVER_H
