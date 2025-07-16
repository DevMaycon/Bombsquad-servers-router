//#include "includes/src/Core/Client.h"
//include "includes/src/Core/Server.h"
#include "includes/src/Core/Proxy.h"
#include <thread>
#include <iostream>

void exec_lobby() {
    int exec_bs = std::system("cd ../../LobbyServer/ && ./bombsquad_server");
    std::cout << "Executing Lobby Finished with code: " << exec_bs << std::endl;
}

int main() {
     // Lobby Server Thread
     std::thread lobby_thread(exec_lobby);
     lobby_thread.detach();

     // Create Proxy Socket
     ProxyServer Proxy(43215);

     // Create Servers
     sockaddr_in Lobby_server_addr = {};
     Lobby_server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
     Lobby_server_addr.sin_port = htons(43211);
     Lobby_server_addr.sin_family = AF_INET;
     Server LobbyServer(0, Lobby_server_addr);

     sockaddr_in game_server_addr = {};
     game_server_addr.sin_family = AF_INET;
     game_server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
     game_server_addr.sin_port = htons(43212);
     Server GameServer(1, game_server_addr);

     Proxy.AddServer(LobbyServer);
     Proxy.AddServer(GameServer);

     // Ready to receive connections
     Proxy.Run();
}
