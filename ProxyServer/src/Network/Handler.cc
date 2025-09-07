#include "includes/src/Network/Handler.h"
#include "includes/src/Core/Client.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <queue>


using namespace NetworkHandler;

void PacketHandler::SetLobby(const Server &lobby) { 
    this->Lobby = lobby; 
    std::cout << "LOBBY ADDRESS: " << inet_ntoa(Lobby.GetAddress().sin_addr) << " PORT: " << ntohs(Lobby.GetAddress().sin_port) << std::endl;
};

int PacketHandler::SendTo(sockaddr_in destination, const char *buffer, int buffer_size) {
    int data_send = sendto(*HandlerSocket, buffer, buffer_size, 0, (sockaddr*) &destination, sizeof(destination));
    return data_send;
};

void PacketHandler::HandleConnection (const char* buffer, int bytes_size, sockaddr_in ClientAddress) {
        std::ofstream log("log.txt", std::ios::app);
        if (log.is_open()) log.write(buffer, bytes_size);

        Packet NetworkPacket;
        NetworkPacket.address = ClientAddress;
        NetworkPacket.header = buffer[0];
        NetworkPacket.data_size = bytes_size;
        memcpy(&NetworkPacket.data, buffer + 1, bytes_size - 1);

        uint16_t port = ntohs(ClientAddress.sin_port);
        uint32_t ip = ClientAddress.sin_addr.s_addr;

        sockaddr_in LobbyAddress = Lobby.GetAddress();
        bool is_from_lobby = (ip == LobbyAddress.sin_addr.s_addr && port == ntohs(LobbyAddress.sin_port));
        bool is_from_server = false;

        Server *server = nullptr;
        for (auto &i_server : Servers) {
            is_from_server = (i_server.GetAddress().sin_addr.s_addr == ip && ntohs(i_server.GetAddress().sin_port) == port);
            if (is_from_server) {
                server = &i_server;
                break;
            }
        }


        if (is_from_lobby) {
            for (auto &[id, client] : Lobby.GetClients()) {
                sockaddr_in caddr = client.GetClientAddress();
                SendTo(caddr, buffer, bytes_size);
            }
        } 

        else if (is_from_server) {
            for (auto& [id, client] : server->GetClients()) {
                sockaddr_in caddr = client.GetClientAddress();
                SendTo(caddr, buffer, bytes_size);
            }
        } 

        else if (Lobby.AddressIsConnected(ClientAddress)) {
            sockaddr_in dest = Lobby.GetAddress();
            SendTo(dest, buffer, bytes_size);
        } 

        else {
            for (auto &server : Servers) {
                if (server.AddressIsConnected(ClientAddress)) {
                    sockaddr_in dest = server.GetAddress();
                    SendTo(dest, buffer, bytes_size);
                    break;
                }
            }
        }

        
        MessageType msg_type = static_cast<MessageType>(NetworkPacket.header);
        switch (msg_type) {
            case MessageType::PING:
                Ping(ClientAddress);
                break;

            case MessageType::CLIENT_JOIN_REQUEST: 
                JoinRequest(NetworkPacket);
                break;

            case MessageType::CLIENT_JOIN_ACCEPTED:
                ClientJoin(NetworkPacket);
                break;

            case MessageType::REDIRECT_ALL: 
                RedirectAll(NetworkPacket); 
                break;

            case MessageType::REDIRECT_PLAYER:
                RedirectPlayer(NetworkPacket);
                break;

            case MessageType::CLIENT_DISCONNECTED: {
                QuitRequest(NetworkPacket);
                break;
            }

            case MessageType::CLIENT_DISCONNECTED_ACK: {
                ClientExit(NetworkPacket);
                break;
            }

            case MessageType::CLIENT_DISCONNECTED_BY_HOST_ACK:
                SendTo(Lobby.GetAddress(), buffer, bytes_size);
                std::cout << "client redirected or kicked." << std::endl;
                break;

            default: break;
        }
    }

void PacketHandler::Ping(sockaddr_in ClientAddress) {
    char pong_response[1] = {static_cast<uint8_t>(MessageType::PONG)};
    SendTo(ClientAddress, pong_response, 1);
};

void PacketHandler::JoinRequest(NetworkHandler::Packet &Packet) {
    ClientJoinRequest client_request;
    client_request.request_id = Packet.data[2];
    client_request.address = Packet.address;


    for (auto &server : Servers) {
        if (server.AddressIsConnected(client_request.address)) {
            server.ClientRequest(client_request);
            return;
        }
    }

    Lobby.ClientRequest(client_request);
    SendTo(Lobby.GetAddress(), Packet.GetRawData(), Packet.data_size);
};

void PacketHandler::QuitRequest(NetworkHandler::Packet &Packet) {
    ClientQuitRequest client_request;
    client_request.client_id = Packet.data[0];
    client_request.address = Packet.address;

    for (auto &server : Servers) {
        if (server.AddressIsConnected(client_request.address)) {
            server.ClientExitRequest(client_request);
            return;
        }
    }

    Lobby.ClientExitRequest(client_request);
};

void PacketHandler::ClientJoin(NetworkHandler::Packet &Packet) {
    uint8_t ClientID = Packet.data[0];
    uint8_t RequestID = Packet.data[1];
    uint8_t ServerID = Packet.data[2];

    auto lobby_queue = Lobby.GetClientsJoinQueue();
    auto client_request = lobby_queue.find(RequestID);

    if (client_request != lobby_queue.end()) {
        ClientJoinRequest client = client_request->second;

        Lobby.ClientJoin(ClientID, RequestID);
        SendTo(client.address, Packet.GetRawData(), Packet.data_size);
        return;
    }

    for (auto &server : Servers) {
        auto server_queue = server.GetClientsJoinQueue();
        auto client_request = server_queue.find(RequestID);

        if (server_queue.find(RequestID) != server_queue.end()) {
            ClientJoinRequest client = client_request->second;
            server.ClientJoin(ClientID, RequestID);
            SendTo(client.address, Packet.GetRawData(), Packet.data_size);
            return;
        }
    }
};

void PacketHandler::ClientExit(NetworkHandler::Packet &Packet) {
    sockaddr_in client_address = Packet.address;
    sockaddr_in lobby_address = Lobby.GetAddress();
    uint8_t ClientID = Packet.data[0];

    auto lobby_queue = Lobby.GetClientsQuitQueue();
    auto client_request = lobby_queue.find(ClientID);

    if (client_request != lobby_queue.end()) {
        ClientQuitRequest client = client_request->second;
        Lobby.ClientExit(ClientID);
        SendTo(client.address, Packet.GetRawData(), Packet.data_size);
        return;
    }

    for (auto &server : Servers) {
        auto server_queue = server.GetClientsQuitQueue();
        auto client_request = server_queue.find(ClientID);

        if (server_queue.find(ClientID) != server_queue.end()) {
            ClientQuitRequest client = client_request->second;
            server.ClientExit(ClientID);
            SendTo(client.address, Packet.GetRawData(), Packet.data_size);
            return;
        }
    }
};

void PacketHandler::RedirectAll(NetworkHandler::Packet &Packet) {
    uint8_t ServerID = Packet.data[0];
    std::cout << "Redirecting all players to server " << (int)ServerID << std::endl;

    std::queue<NetworkHandler::Packet> RedirectQueue;

    for (auto client: Lobby.GetClients()) {
        NetworkHandler::Packet RedirectPacket = Packet;
        RedirectPacket.data[0] = static_cast<uint8_t>(client.first);
        RedirectPacket.data[1] = static_cast<uint8_t>(ServerID);
        RedirectQueue.push(RedirectPacket);
    }

    for (int i = 0; i < RedirectQueue.size() + 1; i++) {
        NetworkHandler::Packet packet = RedirectQueue.front();
        RedirectPlayer(packet);
        RedirectQueue.pop();
    }
};

void PacketHandler::RedirectPlayer(NetworkHandler::Packet &Packet) {
    uint8_t ClientID = Packet.data[0];
    uint8_t ServerID = Packet.data[1];

    if (Lobby.GetClients().find(ClientID) != Lobby.GetClients().end()) {
        auto client = Lobby.GetClients().find(ClientID)->second;

        char buffer[2];
        buffer[0] = static_cast<uint8_t>(MessageType::CLIENT_DISCONNECTED_BY_HOST);
        buffer[1] = static_cast<uint8_t>(ClientID);

        LobbyRedirect(ClientID, ServerID);
        SendTo(client.GetClientAddress(), buffer, 2);
    }
};


int PacketHandler::LobbyRedirect(uint8_t ClientID, uint8_t ServerID) {
    auto &clients = Lobby.GetClients();
    auto it = clients.find(ClientID);
    if (it == clients.end()) return -1;

    sockaddr_in clientAddr = clients.at(ClientID).GetClientAddress();
    Server &server = Servers.at(ServerID);
    server.AddClient(clients.size(), clientAddr);
    Lobby.ClientExit(ClientID);
    return 0;
};
