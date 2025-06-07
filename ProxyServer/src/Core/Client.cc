#include <arpa/inet.h>
#include <includes/src/Core/Client.h>


Client::Client(sockaddr_in &ClientAddress) {
    this->ClientAddress = ClientAddress;
}

int Client::SetClientId(int ClientID) {
    if (this->ClientID = ClientID) {
        return 1;
    }
    return 0;
}

int Client::GetClientId() {
    return ClientID;
}