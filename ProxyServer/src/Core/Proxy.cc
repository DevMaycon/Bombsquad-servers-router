#include "includes/src/Core/Proxy.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>

ProxyServer::ProxyServer(int ProxyPort) {
    this->ProxyPort = ProxyPort;
}

int ProxyServer::Listen() {
    ProxySocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (!ProxySocket) {
        std::cout << "Proxy can't create socket. exiting..." << std::endl;    
        return -1;
    }

    ProxyAddress.sin_family = AF_INET;
    ProxyAddress.sin_port = htons(ProxyPort);
    ProxyAddress.sin_addr.s_addr = INADDR_ANY;

    if (!bind(ProxySocket, (sockaddr*)&ProxyAddress, sizeof(ProxyAddress))) {
        std::cout << "Proxy can't listen port  " << ProxyPort << std::endl;    
        return -1;
    }

    std::cout << "Proxy started in port " << ProxyPort << std::endl;
    return 0;
}

void ProxyServer::Run(int (*HandlerMethod)()) {
    ProxyServer::Listen();

    while (true) {
        HandlerMethod();
    };
}