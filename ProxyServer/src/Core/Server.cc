#include "includes/src/Core/Server.h"
#include "includes/src/Core/Client.h"

Server::Server(const int ServerID, sockaddr_in ServerAddress) {
    this->ServerID = ServerID;
    this->ServerAddress = ServerAddress;
}

std::unordered_map<int, Client> Server::GetClients() {
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

int Server::AddressIsConnected(sockaddr_in address) {
    for (auto& client : Clients) {
        if (
            client.second.GetClientAddress().sin_addr.s_addr == address.sin_addr.s_addr
            && client.second.GetClientAddress().sin_port == address.sin_port 
            ) 
        {
            return 1;
        }
    }
    return 0;
}

int Server::AddClient(sockaddr_in address) {
    if (!IsFull()) {
        Client client(address);
        client.SetClientId(LastClientID);
        Clients.emplace(LastClientID, client);
        LastClientID++;
        return 1;
    }

    return 0;
}

int Server::RemoveClient(sockaddr_in address) {
    for (auto client : Clients) {
        if (
            client.second.GetClientAddress().sin_addr.s_addr == address.sin_addr.s_addr
            && client.second.GetClientAddress().sin_port == address.sin_port
            )
        {
            Clients.erase(client.second.GetClientId());
            return 1;
        }
    }
    return 0;
}
