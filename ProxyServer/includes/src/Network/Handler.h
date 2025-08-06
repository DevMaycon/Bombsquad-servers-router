#ifndef NETWORK_HANDLER
#define NETWORK_HANDLER

#include <arpa/inet.h>
#include <vector>
#include "includes/src/Core/Server.h"

namespace NetworkHandler {
    // Custom Bombsquad Packet Structure
    struct Packet {
        uint8_t header;
        char* data[1024];
    };

    // Proxy Custom Messages ( Packet Size )
    enum class MessageType : int {
        UNKNOWN = -1,

        PING = 0,
        PONG = 1,

        CLIENT_EXIT = 32,
        CLIENT_INFO = 18,
        CLIENT_JOIN = 24,

        REDIRECT_PLAYER = 90,
        REDIRECT_ALL = 91,

        HOST_PACKET = 37
    };

    void HandleConnection(int socket_fd, const char* buffer, const int bytes_size, std::vector<Server> &Servers, sockaddr_in ClientAddress);

    // Functions for Messages
    void Ping(int socket_fd, sockaddr_in ClientAddress);

    void ClientJoin(std::vector<Server> &Servers, sockaddr_in ClientAddress);
    void ClientExit(std::vector<Server> &Servers, sockaddr_in ClientAddress);

    void RedirectAll(Packet &Packet, std::vector<Server> &Servers, sockaddr_in ClientAddress);
    void RedirectPlayer(Packet &Packet, std::vector<Server> &Servers, sockaddr_in ClientAddress);
    
}

#endif
