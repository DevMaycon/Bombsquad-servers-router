#include "includes/src/Network/Handler.h"
#include "includes/src/Core/Client.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <queue>

namespace NetworkHandler {
    void HandleConnection (
            int socket_fd, const char* buffer, int bytes_size,
            std::vector<Server>& Servers, sockaddr_in ClientAddress) 
        {
            std::ofstream log("log.txt", std::ios::app);
            if (log.is_open()) log.write(buffer, bytes_size);

            uint16_t port = ntohs(ClientAddress.sin_port);
            uint32_t ip = ClientAddress.sin_addr.s_addr;

            // 1. Se vem de algum servidor (lobby ou jogo)
            for (auto &server : Servers) {
                sockaddr_in saddr = server.GetAddress();
                if (saddr.sin_addr.s_addr == ip && saddr.sin_port == htons(port)) {
                    for (auto& [id, client] : server.GetClients()) {
                        sockaddr_in caddr = client.GetClientAddress();
                        sendto(socket_fd, buffer, bytes_size, 0, (sockaddr*)&caddr, sizeof(caddr));
                    }
                    return;
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

            // 3. Novo cliente → conecta ao lobby (server 0, porta 43211)
            MessageType msg_type = static_cast<MessageType>(bytes_size);
            Server& lobby = Servers.at(0);
            sockaddr_in lobby_addr = lobby.GetAddress();

            switch (msg_type) {
                case MessageType::CLIENT_JOIN:
                    std::cout << "[JOIN] IP: " << inet_ntoa(ClientAddress.sin_addr)
                            << ":" << port << std::endl;

                    lobby.AddClient(ClientAddress);

                    sendto(socket_fd, buffer, bytes_size, 0,
                        (sockaddr*)&lobby_addr, sizeof(lobby_addr));
                    break;

                case MessageType::CLIENT_EXIT:
                    std::cout << "[EXIT] IP: " << inet_ntoa(ClientAddress.sin_addr)
                            << ":" << port << std::endl;

                    sendto(socket_fd, buffer, bytes_size, 0,
                        (sockaddr*)&lobby_addr, sizeof(lobby_addr));

                    lobby.RemoveClient(ClientAddress);
                    break;

                case MessageType::REDIRECT_ALL: {
                    std::cout << "[REDIRECT_ALL] Enviando todos do lobby para servidor 1." << std::endl;

                    for (auto &server :  Servers) {
                        std::cout
                            << "Server: " << &server
                            << " Server ID:" << server.GetServerID()
                        << std::endl;
                    }
                    Server &destino = Servers.at(1);

                    std::vector<sockaddr_in> clientes_a_mover;

                    // Coleta os clientes do lobby
                    for (auto& [id, client] : lobby.GetClients()) {
                        clientes_a_mover.push_back(client.GetClientAddress());
                    }

                    for (auto& addr : clientes_a_mover) {
                        lobby.RemoveClient(addr);
                        destino.AddClient(addr);

                        std::cout << " -> Movido: " << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port) << std::endl;

                        // (Opcional) avisa o servidor 1 que tem um novo cliente
                        sockaddr_in dest_addr = destino.GetAddress();
                        sendto(socket_fd, buffer, bytes_size, 0,
                            (sockaddr*)&dest_addr, sizeof(dest_addr)
                        );
                    }

                    break;
                    }

                default:
                    std::cout << "[UNKNOWN TYPE] Size: " << bytes_size << std::endl;
                    break;
            }
        }


}
