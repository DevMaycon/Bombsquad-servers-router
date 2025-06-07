#ifndef CLIENT_H
#define CLIENT_H

#include <arpa/inet.h>


class Client {
    private:
        sockaddr_in ClientAddress;
        int ClientID;
    public:
        Client(sockaddr_in &ClientAddress);

        int SetClientId(int ClientID);
        int GetClientId();
};

#endif // CLIENT_H