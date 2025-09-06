#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <vector>

#ifndef HOST_DETECTION
#define HOST_DETECTION

bool host_online(sockaddr_in target_address);
std::vector<int> host_scan_cycle();


#endif // HOST_DETECTION
