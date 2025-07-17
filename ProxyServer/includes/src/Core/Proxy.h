#ifndef PROXY_H
#define PROXY_H

#include <arpa/inet.h>
#include <vector>
#include "includes/src/Network/Handler.h"
#include "includes/src/Core/Server.h"

class ProxyServer {
    private:
        sockaddr_in ProxyAddress;
        int ProxySocket;
        int ProxyPort;
        std::vector<Server> Servers;
        
        int Listen();

    public:
        ProxyServer(int ProxyPort);
        std::vector<Server> GetServers();
        void Run();
        void AddServer(Server Server);


};


#endif // PROXY_H
