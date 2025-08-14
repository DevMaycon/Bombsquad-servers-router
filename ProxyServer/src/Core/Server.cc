#include "includes/src/Core/Server.h"
#include "includes/src/Core/Client.h"
#include <iostream>
#include <cstring>


std::unordered_map<uint8_t, Client> &Server::GetClients() {
    return Clients;
}

std::unordered_map<uint8_t, ClientJoinRequest> Server::GetClientsQueue() {
    return ClientsJoinQueue;
}

sockaddr_in Server::GetAddress() {
    return ServerAddress;
}

bool Server::IsFull() {
    return Clients.size() >= MaxPlayers;
}

int Server::GetServerID() {
   return ServerID;
}

int Server::AddClient(uint8_t ClientID, sockaddr_in ClientAddress) {
        Client client(ClientAddress);
        client.SetClientId(ClientID);

        Clients.emplace(ClientID, client);
        return 0;
}


bool Server::AddressIsConnected(sockaddr_in Address) {
    for (auto &[client_id, client] : Clients) {
        sockaddr_in const client_address = client.GetClientAddress();

        if (client_address.sin_addr.s_addr == Address.sin_addr.s_addr 
            && ntohs(client_address.sin_port) == ntohs(Address.sin_port)) {
            return true;
        }
    }

    return false;
}


int Server::ClientRequest(ClientJoinRequest JoinRequest) {
    ClientsJoinQueue.emplace(JoinRequest.request_id, JoinRequest);
    return 0;
};

int Server::ClientJoin(uint8_t ClientID, uint8_t RequestID) {
    if (!IsFull()) {
        ClientJoinRequest request = ClientsJoinQueue[RequestID];

        AddClient(ClientID, request.address);
        ClientsJoinQueue.erase(RequestID);
        return 0;
    }

    return -1;
}


int Server::ClientExit(uint8_t ClientID) {
    Clients.erase(ClientID);
    return 0;
}