#ifndef PROXY_H
#define PROXY_H

#include <arpa/inet.h>

class ProxyServer {
    private:
        sockaddr_in ProxyAddress;
        int ProxySocket;
        int ProxyPort;
        
        int Listen();

    public:
        ProxyServer(int ProxyPort);

        void Run(int (*HandlerMethod)());
};


#endif // PROXY_H