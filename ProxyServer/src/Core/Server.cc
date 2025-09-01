#include "includes/src/Core/Server.h"
#include "includes/src/Core/Client.h"
#include <iostream>
#include <cstring>


std::unordered_map<uint8_t, Client> &Server::GetClients() {
    return Clients;
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
    if (AddressIsConnected(ClientAddress)) {
        for (auto it = Clients.begin(); it != Clients.end(); ++it) {
            sockaddr_in client_addr = it->second.GetClientAddress();
            if (client_addr.sin_addr.s_addr == ClientAddress.sin_addr.s_addr &&
                ntohs(client_addr.sin_port) == ntohs(ClientAddress.sin_port)) {

                Client client = std::move(it->second);
                client.SetClientId(ClientID);

                Clients.erase(it);
                Clients.emplace(ClientID, std::move(client));
                return 1;
            }
        }
    }

    Client client(ClientAddress);
    client.SetClientId(ClientID);
    Clients.emplace(ClientID, std::move(client));
    return 1;
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

int Server::ClientExitRequest(ClientQuitRequest QuitRequest) {
    ClientsQuitQueue.emplace(QuitRequest.client_id, QuitRequest);
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
    ClientsQuitQueue.erase(ClientID);
    Clients.erase(ClientID);
    return 0;
}