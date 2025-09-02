#ifndef NETWORK_HANDLER
#define NETWORK_HANDLER

#include "includes/src/Core/Server.h"
#include <arpa/inet.h>
#include <vector>
#include <cstring>


namespace NetworkHandler {

    // Custom Bombsquad Packet Structure
    struct Packet {
        sockaddr_in address;
        uint8_t header;
        char data[1024];
        int data_size;
        char raw_data[1024];

        char *GetRawData() { 
            memcpy(raw_data, &header, 1);
            memcpy(raw_data + 1, data, data_size - 1);
            return raw_data;
        };
    };

    // Proxy Custom Messages ( Packet Size )
    enum class MessageType : int {
        UNKNOWN = -1,

        PING = 11,
        PONG = 12,

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

        void JoinRequest(NetworkHandler::Packet &Packet);
        void ClientJoin(NetworkHandler::Packet &Packet);

        void QuitRequest(NetworkHandler::Packet &Packet);
        void ClientExit(NetworkHandler::Packet &Packet);

        void RedirectAll(NetworkHandler::Packet &Packet);
        void RedirectPlayer(NetworkHandler::Packet &Packet);

        int LobbyRedirect(uint8_t ClientID, uint8_t ServerID);
};

#endif
