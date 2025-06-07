#include "includes/src/Core/Server.h"
#include "includes/src/Core/Client.h"

Server::Server(const int ServerID) {
    this->ServerID = ServerID;
}

bool Server::IsFull() {
    return Clients.size() >= MaxPlayers;
}

int Server::AddClient(Client client) {
    if (!IsFull()) {
        Clients.emplace(client.GetClientId(), client);
        return 1;
    }

    return 0;
}

int Server::RemoveClient(Client client) {
    if (Clients.find(client.GetClientId()) != Clients.end()) {
        Clients.erase(client.GetClientId());
        return 1;
    }

    return 0;
}