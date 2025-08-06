#include "includes/src/Network/Handler.h"
#include "includes/src/Core/Client.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <queue>
#include <cstring>

namespace NetworkHandler {
    void HandleConnection (
            int socket_fd, const char* buffer, int bytes_size,
            std::vector<Server>& Servers, sockaddr_in ClientAddress) 
        {
            std::ofstream log("log.txt", std::ios::app);
            if (log.is_open()) log.write(buffer, bytes_size);

            Packet NetworkPacket;
            NetworkPacket.header = buffer[0];
            memcpy(NetworkPacket.data, buffer + 1, bytes_size - 1);

            uint16_t port = ntohs(ClientAddress.sin_port);
            uint32_t ip = ClientAddress.sin_addr.s_addr;

            Server& lobby = Servers.at(0);
            sockaddr_in lobby_addr = lobby.GetAddress();

            // 1. Se vem de algum servidor (lobby ou jogo)
            for (auto &server : Servers) {
                sockaddr_in saddr = server.GetAddress();
                if (saddr.sin_addr.s_addr == ip && saddr.sin_port == htons(port)) {
                    for (auto& [id, client] : server.GetClients()) {
                        sockaddr_in caddr = client.GetClientAddress();
                        sendto(socket_fd, buffer, bytes_size, 0, (sockaddr*)&caddr, sizeof(caddr));
                    }
                }
            }
            // 2. Já conectado a algum servidor (repassa pra lá)
            for (auto &server : Servers) {
                if (server.AddressIsConnected(ClientAddress)) {
                    sockaddr_in dest = server.GetAddress();
                    sendto(socket_fd, buffer, bytes_size, 0, (sockaddr*)&dest, sizeof(dest));
                    return;
                }
            }

            MessageType msg_type = static_cast<MessageType>(NetworkPacket.header);
            switch (msg_type) {
                case MessageType::PING:
                    Ping(socket_fd, ClientAddress);
                    break;

                case MessageType::CLIENT_JOIN:
                    ClientJoin(Servers, ClientAddress);
                    break;

                case MessageType::REDIRECT_ALL: 
                    RedirectAll(NetworkPacket, Servers, ClientAddress); 
                    break;

                default: break;
            }
        }

    void Ping(int socket_fd, sockaddr_in ClientAddress) {
        char pong_response[1] = {static_cast<uint8_t>(MessageType::PONG)};
        sendto(socket_fd, pong_response, 1, 0, (sockaddr*)&ClientAddress, sizeof(ClientAddress));
    };

    void ClientJoin(std::vector<Server> &Servers, sockaddr_in ClientAddress) {
        Server &lobby = Servers.at(0);
        lobby.AddClient(ClientAddress);
    };

    void ClientExit(std::vector<Server> &Servers, sockaddr_in ClientAddress) {};

    void RedirectAll(Packet &Packet, std::vector<Server> &Servers, sockaddr_in ClientAddress) {
        Server &destination = Servers.at(1);
        Server &lobby = Servers.at(0);

        // Coleta os clientes do lobby
        for (auto& [id, client] : lobby.GetClients()) {
            lobby.RemoveClient(client.GetClientAddress());
            destination.AddClient(client.GetClientAddress());
        }
    };
    
    void RedirectPlayer(Packet &Packet, std::vector<Server> &Servers, sockaddr_in ClientAddress) {};
}
