#ifndef NETWORK_HANDLER
#define NETWORK_HANDLER

#include <arpa/inet.h>
#include <vector>
#include "includes/src/Core/Server.h"

namespace NetworkHandler {
    // Proxy Custom Messages ( Packet Size )
    enum class MessageType : int {
        UNKNOWN = 0,
        PING = 1,
        CLIENT_EXIT = 2,
        CLIENT_JOIN = 40,
        REDIRECT_ALL = 99  // nova mensagem de redirecionamento
    };

    void HandleConnection(int socker_fd, const char* buffer, const int bytes_size, std::vector<Server> &Servers, sockaddr_in ClientAddress);
}

#endif
