#include <arpa/inet.h>
#include <includes/src/Core/Client.h>


Client::Client(const sockaddr_in &ClientAddress) {
    this->ClientAddress = ClientAddress;
}

sockaddr_in Client::GetClientAddress() {
    return ClientAddress;
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