// main.cpp
#include <FL/Fl.H>
#include <iostream>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "state_machine.h"
#include "messenger_window.h"
#include "configuration.h"

int sockfd;
struct sockaddr_in servaddr;
Configuration config;

int main(int argc, char **argv) {
    Fl::lock();  // Initialize FLTK's multithreading support
    config.read();  // Read configuration from file
    std::cout << "Starting fltk messenger with configuration:"
        << "destination_ip=" << config.destination_ip << ", receiving_port="
        << config.receiving_port << ", sending_port" << config.sending_port
        << std::endl;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return -1;
    }
    
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(config.destination_ip.c_str());
    servaddr.sin_port = htons(config.sending_port);
    
    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(config.receiving_port);

    if (bind(sockfd, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
        std::cerr << "Failed to bind socket" << std::endl;
        close(sockfd);
        return 1;
    }

    StateMachine* state_machine = new StateMachine(sockfd, servaddr);
    std::thread state_machine_thread(&StateMachine::run, state_machine);

    MessengerWindow window(400, 340, "FLTK Encrypted Chat", state_machine);
    window.run();
    
    delete state_machine;
    close(sockfd);
    
    state_machine_thread.join();
    
    return 0;
}

