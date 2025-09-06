#include "includes/src/Core/Proxy.h"
#include "includes/src/Main/hosts_detection.h"
#include <thread>
#include <iostream>


void exec_lobby() {
    int exec_bs = std::system("cd ../../LobbyServer/ && ./bombsquad_server");
    std::cout << "Executing Lobby Finished with code: " << exec_bs << std::endl;
}

int main() {
    std::vector<int> bombsquad_servers_ports = host_scan_cycle();
    std::unordered_map<int, Server> scanned_servers;

    // Lobby Server Thread
    std::thread lobby_thread(exec_lobby);
    lobby_thread.detach();

    // Create Proxy Socket
    ProxyServer Proxy(43215);

    // Create Default Lobby Server
    sockaddr_in Lobby_server_addr = {};
    Lobby_server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    Lobby_server_addr.sin_port = htons(43210);
    Lobby_server_addr.sin_family = AF_INET;

    Server LobbyServer(0, Lobby_server_addr);
    Proxy.GetProxyHandler().SetLobby(LobbyServer);

    // Create servers based in port scan
    for (auto it = bombsquad_servers_ports.begin(); it != bombsquad_servers_ports.end(); ++it ) {
        if (scanned_servers.find(*it) == scanned_servers.end()) {
            sockaddr_in server_addr = {};
            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(*it);
            server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

            // Add server in list
            Server GameServer(std::distance(bombsquad_servers_ports.begin(), it)+1, server_addr);
            scanned_servers.emplace(*it, GameServer);
            Proxy.AddServer(GameServer);
        }
    }

    // Ready to receive connections
    Proxy.Run();
}
