#ifndef NETWORK_HANDLER
#define NETWORK_HANDLER

#include <arpa/inet.h>
#include <vector>
#include "includes/src/Core/Server.h"


namespace NetworkHandler {

    // Custom Bombsquad Packet Structure
    struct Packet {
        uint8_t header;
        char data[1024];
        int data_size;
        char raw_data[1024];
    };

    // Proxy Custom Messages ( Packet Size )
    enum class MessageType : int {
        UNKNOWN = -1,

        PING = 0,
        PONG = 1,

        CLIENT_EXIT = 32,
        CLIENT_INFO = 18,
        CLIENT_JOIN_REQUEST  = 24,
        CLIENT_JOIN_ACCEPTED = 25,
        CLIENT_DISCONNECTED = 32,
        CLIENT_DISCONNECTED_ACK = 33,
        CLIENT_DISCONNECTED_BY_HOST = 34,
        CLIENT_DISCONNECTED_BY_HOST_ACK = 35,

        REDIRECT_PLAYER = 90,
        REDIRECT_ALL = 91,

        HOST_PACKET = 37
    };
};

class PacketHandler {
    private:
        std::vector<Server> &Servers;
        int *HandlerSocket;
        Server Lobby;

    public:
        PacketHandler(std::vector<Server> &Servers, int &HandlerSocket) : Servers(Servers), HandlerSocket(&HandlerSocket) {};
    
        int SendTo(sockaddr_in destination, const char* buffer, int buffer_size);
        void HandleConnection(const char* buffer, const int bytes_size, sockaddr_in ClientAddress);

        void SetLobby(const Server &lobby);
        Server &GetLobby() { return this->Lobby; };

        // Functions for Messages
        void Ping(sockaddr_in ClientAddress);

        void ClientRequest(sockaddr_in ClientAddress, NetworkHandler::Packet &Packet);
        void ClientJoin(NetworkHandler::Packet &Packet);
        void ClientExit(NetworkHandler::Packet &Packet);

        void RedirectAll(NetworkHandler::Packet &Packet);
        void RedirectPlayer(NetworkHandler::Packet &Packet);

        int LobbyRedirect(uint8_t ClientID, uint8_t ServerID);
};

#endif
