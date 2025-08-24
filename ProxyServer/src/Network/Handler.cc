#include "includes/src/Network/Handler.h"
#include "includes/src/Core/Client.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <queue>
#include <cstring>

using namespace NetworkHandler;

void PacketHandler::SetLobby(const Server &lobby) { 
    this->Lobby = lobby; 
    std::cout << "LOBBY ADDRESS: " << inet_ntoa(Lobby.GetAddress().sin_addr) << "\nPORT: " << ntohs(Lobby.GetAddress().sin_port) << std::endl;
};

int PacketHandler::SendTo(sockaddr_in destination, const char *buffer, int buffer_size) {
    int data_send = sendto(*HandlerSocket, buffer, buffer_size, 0, (sockaddr*) &destination, sizeof(destination));
    return data_send;
};

void PacketHandler::HandleConnection (const char* buffer, int bytes_size, sockaddr_in ClientAddress) {
        std::ofstream log("log.txt", std::ios::app);
        if (log.is_open()) log.write(buffer, bytes_size);

        Packet NetworkPacket;
        NetworkPacket.header = buffer[0];
        NetworkPacket.data_size = bytes_size;
        memcpy(&NetworkPacket.data, buffer + 1, bytes_size - 1);
        memcpy(&NetworkPacket.raw_data, buffer, bytes_size);

        uint16_t port = ntohs(ClientAddress.sin_port);
        uint32_t ip = ClientAddress.sin_addr.s_addr;

        sockaddr_in LobbyAddress = Lobby.GetAddress();
        bool is_from_lobby = (ip == LobbyAddress.sin_addr.s_addr && port == ntohs(LobbyAddress.sin_port));
        bool is_from_server = false;

        Server *server = nullptr;
        for (auto &i_server : Servers) {
            is_from_server = (i_server.GetAddress().sin_addr.s_addr == ip && ntohs(i_server.GetAddress().sin_port) == port);
            server = &i_server;
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

        else if (GetLobby().AddressIsConnected(ClientAddress)) {
            sockaddr_in dest = Lobby.GetAddress();
            SendTo(dest, buffer, bytes_size);
            return;
        } 

        else {
            for (auto &server : Servers) {
                if (server.AddressIsConnected(ClientAddress)) {
                    sockaddr_in dest = server.GetAddress();
                    SendTo(dest, buffer, bytes_size);
                    return;
                }
            }
        }

        
        MessageType msg_type = static_cast<MessageType>(NetworkPacket.header);
        switch (msg_type) {
            case MessageType::PING:
                Ping(ClientAddress);
                break;

            case MessageType::CLIENT_JOIN_REQUEST: {
                int server_response = SendTo(GetLobby().GetAddress(), buffer, bytes_size);
                if (server_response != -1) { ClientRequest(ClientAddress, NetworkPacket); }
                break;
            }

            case MessageType::CLIENT_JOIN_ACCEPTED: 
                ClientJoin(NetworkPacket);
                break;

            case MessageType::REDIRECT_ALL: 
                RedirectAll(NetworkPacket); 
                break;

            case MessageType::REDIRECT_PLAYER:
                RedirectPlayer(NetworkPacket);
                break;

            case MessageType::CLIENT_DISCONNECTED_BY_HOST_ACK:
                SendTo(Lobby.GetAddress(), buffer, bytes_size);
                break;

            default: break;
        }
    }

void PacketHandler::Ping(sockaddr_in ClientAddress) {
    char pong_response[1] = {static_cast<uint8_t>(MessageType::PONG)};
    SendTo(ClientAddress, pong_response, 1);
};

void PacketHandler::ClientRequest(sockaddr_in ClientAddress, NetworkHandler::Packet &Packet) {
    ClientJoinRequest client_request;
    client_request.request_id = Packet.data[2];
    client_request.address = ClientAddress;

    Lobby.ClientRequest(client_request);
};

void PacketHandler::ClientJoin(NetworkHandler::Packet &Packet) {
    uint8_t ClientID = Packet.data[0];
    uint8_t RequestID = Packet.data[1];
    uint8_t ServerID = Packet.data[2];

    auto lobby_queue = Lobby.GetClientsQueue();

    auto client_request = lobby_queue.find(RequestID);
    if (client_request != lobby_queue.end()) {
        ClientJoinRequest client = client_request->second;

        GetLobby().ClientJoin(ClientID, RequestID);
        SendTo(client.address, Packet.raw_data, Packet.data_size);
    }
};

void PacketHandler::ClientExit(NetworkHandler::Packet &Packet) {
    uint8_t ClientID = Packet.data[0];
    Lobby.ClientExit(ClientID);
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
        auto client = GetLobby().GetClients().find(ClientID)->second;

        char buffer[2];
        buffer[0] = static_cast<uint8_t>(MessageType::CLIENT_DISCONNECTED_BY_HOST);
        buffer[1] = static_cast<uint8_t>(ClientID);

        LobbyRedirect(ClientID, ServerID);
        SendTo(client.GetClientAddress(), buffer, 2);
    }
};


int PacketHandler::LobbyRedirect(uint8_t ClientID, uint8_t ServerID) {
    auto &clients = GetLobby().GetClients();
    auto it = clients.find(ClientID);
    if (it == clients.end()) return -1;

    sockaddr_in clientAddr = clients.at(ClientID).GetClientAddress();
    Server &server = Servers.at(ServerID);
    server.AddClient(clients.size(), clientAddr);
    Lobby.ClientExit(ClientID);
    return 0;
};