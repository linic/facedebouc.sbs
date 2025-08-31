#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <condition_variable>
#include <sodium.h>
#include "state_machine.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <local_port> <remote_port> <remote_ip>" << std::endl;
        return 1;
    }

    if (sodium_init() < 0) {
        std::cerr << "Failed to initialize libsodium" << std::endl;
        return 1;
    }

    int local_port = std::stoi(argv[1]);
    int remote_port = std::stoi(argv[2]);
    std::string remote_ip = argv[3];

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    struct sockaddr_in local_addr, remote_addr;
    std::memset(&local_addr, 0, sizeof(local_addr));
    std::memset(&remote_addr, 0, sizeof(remote_addr));

    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(local_port);

    if (bind(sock, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
        std::cerr << "Failed to bind socket" << std::endl;
        close(sock);
        return 1;
    }

    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = inet_addr(remote_ip.c_str());
    remote_addr.sin_port = htons(remote_port);

    StateMachine state_machine(sock, remote_addr);
    state_machine.run();

    close(sock);
    return 0;
}
