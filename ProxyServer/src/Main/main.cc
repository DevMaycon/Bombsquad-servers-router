#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ostream>
#include <iostream>
#include <unistd.h>
#include <cstring>

int main() {
     int server = socket(AF_INET, SOCK_DGRAM, 0);

     sockaddr_in serverAddress, clientAddress;
     serverAddress.sin_family = AF_INET;
     serverAddress.sin_port = htons(5000);
     serverAddress.sin_addr.s_addr = INADDR_ANY;

     bind(server, (struct sockaddr*) &serverAddress, sizeof(serverAddress));

     while (true) {
          char buffer[1024] = {};
          socklen_t socklen = sizeof(clientAddress);
          int response = recvfrom(server, buffer, sizeof(buffer),
                         0, (struct sockaddr*) &clientAddress, &socklen);

          std::cout << inet_ntoa(clientAddress.sin_addr) << ":" << clientAddress.sin_port << std::endl;
     }

     close(server);
     return 0;
}
