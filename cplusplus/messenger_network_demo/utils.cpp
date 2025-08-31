#include "utils.h"

void print_as_hex(unsigned char* data, int length) {
    for (int i = 0; i < length; i++) {
        std::cout 
              << std::uppercase 
              << std::hex << (0xFF & data[i]) << " ";
    }
    std::cout << std::endl;
}

