#include "includes/src/Network/Handler.h"
#include "includes/src/Core/Proxy.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>

ProxyServer::ProxyServer(int ProxyPort) {
    this->ProxyPort = ProxyPort;
}

int ProxyServer::Listen() {
    ProxySocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (ProxySocket == -1) {
        perror("Proxy can't create socket. exiting...");
        return -1;
    }

    ProxyAddress.sin_family = AF_INET;
    ProxyAddress.sin_port = htons(ProxyPort);
    ProxyAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(ProxySocket, (sockaddr*)&ProxyAddress, sizeof(ProxyAddress))  == -1) {
        perror("Proxy failed in listen port");
        return -1;
    }

    std::cout << "Proxy started in port " << ProxyPort << std::endl;
    return 0;
}

void ProxyServer::AddServer(Server server) {
    Servers.push_back(server);
}

std::vector<Server> ProxyServer::GetServers() {
    return Servers;
}


void ProxyServer::Run() {
    if ( ProxyServer::Listen() == -1 ) {
        perror("Failed to open proxy server");
    }

    while (true) {
        char buffer[1024];
        sockaddr_in ClientAddress;
        socklen_t ClientLen = sizeof(ClientAddress);

        int recv_bytes = recvfrom(
            ProxySocket, buffer, sizeof(buffer), 0,
            (sockaddr*) &ClientAddress, &ClientLen
        );

        NetworkHandler::HandleConnection(ProxySocket, buffer, recv_bytes, Servers, ClientAddress);
    };
}
