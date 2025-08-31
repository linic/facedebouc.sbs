#include "state_machine.h"
#include "utils.h"
#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sodium.h>

StateMachine::StateMachine(int socket, struct sockaddr_in& remote) 
    : current_state(State::Initialization), sock(socket), remote_addr(remote), 
      input_messages(10), output_messages(10), received_packets(10),
      sending_key(true), listening(true) {}

void StateMachine::run() {
    initializeState();
    keyExchangeState();
    sessionEstablishedState();
}

void StateMachine::initializeState() {
    std::cout << "Entering Initialization State" << std::endl;
    listening = true;
    listen_thread = std::thread(&StateMachine::listenForPackets, this);
    current_state = State::KeyExchange;
}

void StateMachine::keyExchangeState() {
    std::cout << "Entering Key Exchange State" << std::endl;
    crypto_kx_keypair(keys.public_key, keys.secret_key);
    print_as_hex(keys.public_key, crypto_kx_PUBLICKEYBYTES);
    std::cout << "//keys.public_key\n";
    print_as_hex(keys.secret_key, crypto_kx_SECRETKEYBYTES);
    std::cout << "//keys.secret_key\n";

    key_send_thread = std::thread(&StateMachine::sendPublicKey, this);
    key_receive_thread = std::thread(&StateMachine::receivePublicKey, this);

    key_receive_thread.join();
    std::this_thread::sleep_for(std::chrono::seconds(3));
    sending_key = false;
    key_send_thread.join();
    print_as_hex(keys.destinator_public_key, crypto_kx_PUBLICKEYBYTES);
    std::cout << "//destinator_public_key\n";
    // sodium explains https://doc.libsodium.org/quickstart
	// #how-can-a-and-b-securely-communicate-without-a-pre-shared-secret-key
    // I have 2 clients and they can't negotiate who the server and the client is going to be since
    // they would need to do that before establishing the communication, but they need to establish
	// the communication first. So I can generate the client session keys and server session keys
	// in both clients. For each message to be sent, it will be sent twice. Once using the
	// client_tx as the encryption key the other with the server_tx. The destinator will try to
	// decrypt with the client_rx. Half the time it will work. When it fails, I can ignore the
	// failure since I know the message comes in twice. Since I'm using UDP, I could decrypt using
	// the server_rx when I get a failure using the client_rx and I could use the copied message
	// in the rare cases when the first message is dropped somewhere.
    if (crypto_kx_client_session_keys(keys.client_rx, keys.client_tx, keys.public_key, keys.secret_key, keys.destinator_public_key) != 0) {
        std::cerr << "Failed to generate client session keys" << std::endl;
        exit(1);
    }
    if (crypto_kx_server_session_keys(keys.server_rx, keys.server_tx, keys.public_key, keys.secret_key, keys.destinator_public_key) != 0) {
        std::cerr << "Failed to generate server session keys" << std::endl;
        exit(1);
    }
    print_as_hex(keys.client_rx, crypto_kx_SESSIONKEYBYTES);
    std::cout << "//keys.client_rx\n";
    print_as_hex(keys.client_tx, crypto_kx_SESSIONKEYBYTES);
    std::cout << "//keys.client_tx\n";

    current_state = State::SessionEstablished;
}

void StateMachine::processAndSendInput(const std::vector<char>& input, const unsigned char* tx_key) {
    unsigned char encrypted[BUFFER_SIZE + crypto_secretbox_MACBYTES];
    unsigned char nonce[crypto_secretbox_NONCEBYTES];
    
    randombytes_buf(nonce, crypto_secretbox_NONCEBYTES);
    print_as_hex(nonce, crypto_secretbox_NONCEBYTES);
    std::cout << "//nonce\n";
    
    crypto_secretbox_easy(reinterpret_cast<unsigned char*>(encrypted),
                          reinterpret_cast<const unsigned char*>(input.data()),
                          input.size(), nonce, tx_key);
    print_as_hex(encrypted, input.size());
    std::cout << "//encrypted\n";
    
    std::vector<char> message(crypto_secretbox_NONCEBYTES + input.size() + crypto_secretbox_MACBYTES);
    std::memcpy(message.data(), nonce, crypto_secretbox_NONCEBYTES);
    std::memcpy(message.data() + crypto_secretbox_NONCEBYTES, encrypted, input.size() + crypto_secretbox_MACBYTES);
    
    sendto(sock, message.data(), message.size(), 0, (struct sockaddr*)&remote_addr, sizeof(remote_addr));
}

void StateMachine::sessionEstablishedState() {
    std::cout << "Entering Session Established State" << std::endl;
    decrypt_thread = std::thread(&StateMachine::decryptAndPrint, this);

    while (listening) {
        std::vector<char> input = input_messages.pop();
        processAndSendInput(input, keys.client_tx);
        processAndSendInput(input, keys.server_tx);
    }

    decrypt_thread.join();
    listen_thread.join();
}

void StateMachine::pushInputMessage(const std::vector<char>& message) {
    input_messages.push(message);
}

std::vector<char> StateMachine::popOutputMessage() {
    return output_messages.pop();
}

void StateMachine::listenForPackets() {
    char buffer[BUFFER_SIZE + crypto_secretbox_NONCEBYTES + crypto_secretbox_MACBYTES];
    struct sockaddr_in sender_addr;
    socklen_t sender_addr_len = sizeof(sender_addr);

    while (listening) {
        std::cout << "listening for packets\n";
        int recv_len = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&sender_addr, &sender_addr_len);
        if (recv_len > 0) {
            std::vector<char> packet(buffer, buffer + recv_len);
            received_packets.push(packet);
        }
    }
    std::cout << "listening for packets stopped\n";
}

void StateMachine::sendPublicKey() {
    while (sending_key) {
        sendto(sock, keys.public_key, sizeof(keys.public_key), 0, (struct sockaddr*)&remote_addr, sizeof(remote_addr));
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void StateMachine::receivePublicKey() {
    std::vector<char> packet = received_packets.pop();
    if (packet.size() == crypto_kx_PUBLICKEYBYTES) {
        std::memcpy(keys.destinator_public_key, packet.data(), crypto_kx_PUBLICKEYBYTES);
        std::cout << "Received public key from remote" << std::endl;
    } else {
        std::cerr << "Received invalid public key" << std::endl;
        exit(1);
    }
}

void StateMachine::decryptAndPrint() {
    char decrypted[BUFFER_SIZE];
    while (listening) {
        std::vector<char> packet = received_packets.pop();
        if (packet.size() > crypto_secretbox_NONCEBYTES + crypto_secretbox_MACBYTES) {
            unsigned char nonce[crypto_secretbox_NONCEBYTES];
            std::memcpy(nonce, packet.data(), crypto_secretbox_NONCEBYTES);
            print_as_hex(nonce, crypto_secretbox_NONCEBYTES);
            std::cout << "//nonce\n";
            unsigned char* ciphertext = reinterpret_cast<unsigned char*>(packet.data() + crypto_secretbox_NONCEBYTES);
            print_as_hex(ciphertext, packet.size() - crypto_secretbox_NONCEBYTES);
            std::cout << "//ciphertext\n";
            int ciphertext_len = packet.size() - crypto_secretbox_NONCEBYTES;
            std::cout << ciphertext_len << "//ciphertext_len\n";

            if (crypto_secretbox_open_easy(reinterpret_cast<unsigned char*>(decrypted),
                                           ciphertext, ciphertext_len, nonce, keys.client_rx) == 0) {
                int decrypted_len = ciphertext_len - crypto_secretbox_MACBYTES;
                std::vector<char> decrypted_message(decrypted, decrypted + decrypted_len);
                output_messages.push(decrypted_message);
            } else {
                // Ignore decryption failures for now since we will fail 50% of the time because
                // we're using both the server_rx and client_rx when sending.
                //std::cerr << "Decryption failed" << std::endl;
            }
        }
    }
}

