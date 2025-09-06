#include "includes/src/Main/hosts_detection.h"
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

int sd = socket(AF_INET, SOCK_DGRAM, 0);

std::vector<int> get_udp_ports() {
    std::vector<int> ports;
    std::ifstream file("/proc/net/udp");
    std::string line;

    if (!file.is_open()) return ports;

    std::getline(file, line); // pula header

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string sl, local_address, rem_address, st;
        int dummy;

        // /proc/net/udp tem várias colunas, pegamos a 2ª (local_address) e 4ª (state)
        if (!(iss >> sl >> local_address >> rem_address >> st)) continue;

        if (st != "07") continue; // 07 = LISTEN (UDP)

        // local_address é no formato IP:PORT em hexadecimal
        auto pos = local_address.find(':');
        if (pos == std::string::npos) continue;

        std::string port_hex = local_address.substr(pos + 1);
        int port = 0;
        std::stringstream ss;
        ss << std::hex << port_hex;
        ss >> port;

        if (port > 1024) {
            ports.push_back(port);
        }
    }

    return ports;
}

bool host_online(sockaddr_in target_address) {
    unsigned char send_data_buffer[] = {0x0d, 0x7b, 0x7d};

    if (sd == -1) { return false; }

    timeval timeout{};
    timeout.tv_sec = 0;
    timeout.tv_usec = 35 * 1000;
    setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    socklen_t target_len = sizeof(target_address);
    int send_data = sendto(sd,
        send_data_buffer, sizeof(send_data_buffer), 0,
        (sockaddr*) &target_address, sizeof(target_address)
    );

    if (send_data == -1) { return false; }

    char recv_buffer[1024];
    sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);

    int recv_bytes = recvfrom(
        sd, recv_buffer, sizeof(recv_buffer), 0,
        (sockaddr*) &client_address, &client_len
    );

    if (recv_bytes == -1) { return false; }

    return true;
}


std::vector<int> host_scan_cycle() {
    std::vector<int> hosts_ports = {};

    for (auto port : get_udp_ports()) {
        sockaddr_in target;
        target.sin_family = AF_INET;
        target.sin_port = htons(port);
        inet_pton(AF_INET, "127.0.0.1", &target.sin_addr);

        if (host_online(target)) {
            hosts_ports.emplace_back(port);
            std::cout << "BOMBSQUAD SERVER OPEN IN PORT -> " << port << std::endl;
        }
    }

    return hosts_ports;
}

