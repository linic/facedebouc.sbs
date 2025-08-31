#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <atomic>
#include <sodium.h>
#include <thread>
#include <netinet/in.h>
#include "thread_safe_fifo.h"

#define BUFFER_SIZE 1024

struct Keys {
    unsigned char public_key[crypto_kx_PUBLICKEYBYTES];
    unsigned char secret_key[crypto_kx_SECRETKEYBYTES];
    unsigned char destinator_public_key[crypto_kx_PUBLICKEYBYTES];
    unsigned char client_rx[crypto_kx_SESSIONKEYBYTES];
    unsigned char client_tx[crypto_kx_SESSIONKEYBYTES];
    unsigned char server_rx[crypto_kx_SESSIONKEYBYTES];
    unsigned char server_tx[crypto_kx_SESSIONKEYBYTES];
};

enum class State {
    Initialization,
    KeyExchange,
    SessionEstablished
};

class StateMachine {
private:
    State current_state;
    int sock;
    struct sockaddr_in remote_addr;
    Keys keys;
    ThreadSafeFIFO received_packets;
    std::atomic<bool> sending_key;
    std::atomic<bool> listening;
    std::thread listen_thread;
    std::thread key_send_thread;
    std::thread key_receive_thread;
    std::thread decrypt_thread;

    void initializeState();
    void keyExchangeState();
    void sessionEstablishedState();
    void processAndSendInput(const std::string& input, const unsigned char* tx_key);
    void listenForPackets();
    void sendPublicKey();
    void receivePublicKey();
    void decryptAndPrint();

public:
    StateMachine(int socket, struct sockaddr_in& remote);
    void run();
};

#endif // STATE_MACHINE_H

