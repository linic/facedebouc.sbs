#include <array>
#include <fstream>
#include <iostream>
#include <sodium.h>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

void generateAndSaveKeyPair(const std::string& pubKeyFile, const std::string& privKeyFile) {
    unsigned char pk[crypto_sign_PUBLICKEYBYTES];
    unsigned char sk[crypto_sign_SECRETKEYBYTES];

    crypto_sign_keypair(pk, sk);

    std::ofstream pubFile(pubKeyFile, std::ios::binary);
    pubFile.write(reinterpret_cast<const char*>(pk), crypto_sign_PUBLICKEYBYTES);

    std::ofstream privFile(privKeyFile, std::ios::binary);
    privFile.write(reinterpret_cast<const char*>(sk), crypto_sign_SECRETKEYBYTES);
}

std::vector<unsigned char> readFile(const std::string& filename) {
    // Open the file
    std::ifstream file(filename, std::ios::binary);
    
    if (!file) {
        throw std::runtime_error("Unable to open file: " + filename);
    }

    // Get the file size
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read the data
    std::vector<unsigned char> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        throw std::runtime_error("Error reading file: " + filename);
    }

    return buffer;
}

void writeFile(const std::string& filename, const std::vector<unsigned char>& data) {
    // Open the file
    std::ofstream file(filename, std::ios::binary);

    if (!file) {
        throw std::runtime_error("Unable to open file for writing: " + filename);
    }

    // Write the data
    if (!file.write(reinterpret_cast<const char*>(data.data()), data.size())) {
        throw std::runtime_error("Error writing to file: " + filename);
    }

    // Ensure all data is written to the file
    file.flush();

    if (!file) {
        throw std::runtime_error("Error occurred while writing to file: " + filename);
    }

    // File will be closed automatically when it goes out of scope
}

std::pair<std::array<unsigned char, crypto_scalarmult_BYTES>, std::array<unsigned char, crypto_scalarmult_BYTES>>
convertEd25519KeysToX25519(const std::vector<unsigned char>& pubKey, const std::vector<unsigned char>& privKey) {
    std::array<unsigned char, crypto_scalarmult_BYTES> x25519_pubkey;
    std::array<unsigned char, crypto_scalarmult_BYTES> x25519_privkey;

    if (crypto_sign_ed25519_pk_to_curve25519(x25519_pubkey.data(), pubKey.data()) != 0) {
        throw std::runtime_error("Failed to convert public key to X25519");
    }

    if (crypto_sign_ed25519_sk_to_curve25519(x25519_privkey.data(), privKey.data()) != 0) {
        throw std::runtime_error("Failed to convert private key to X25519");
    }

    return {x25519_pubkey, x25519_privkey};
}

void encryptFile(const std::string& privKeyFile, const std::string& pubKeyFile, const std::string& inputFile, const std::string& outputFile) {
    // Read the private key
    std::vector<unsigned char> privKey = readFile(privKeyFile);
    if (privKey.size() != crypto_sign_SECRETKEYBYTES) {
        throw std::runtime_error("Invalid private key size");
    }

    // Read the public key
    std::vector<unsigned char> pubKey = readFile(pubKeyFile);
    if (pubKey.size() != crypto_sign_PUBLICKEYBYTES) {
        throw std::runtime_error("Invalid public key size");
    }

    // Read the input file
    std::vector<unsigned char> plaintext = readFile(inputFile);

    // Generate a random nonce
    unsigned char nonce[crypto_box_NONCEBYTES];
    randombytes_buf(nonce, sizeof nonce);

    // Convert Ed25519 keys to X25519 keys
    auto [x25519_pubkey, x25519_privkey] = convertEd25519KeysToX25519(pubKey, privKey);

    // Prepare the output buffer
    std::vector<unsigned char> ciphertext(crypto_box_MACBYTES + plaintext.size());

    // Encrypt the message
    if (crypto_box_easy(
                ciphertext.data(),
                plaintext.data(),
                plaintext.size(),
                nonce,
                x25519_pubkey.data(), // .data() to get C-style pointers.
                x25519_privkey.data()) // .data() same reason
        != 0) {
        throw std::runtime_error("Encryption failed");
    }

    // Prepare the final output: nonce + ciphertext
    std::vector<unsigned char> finalOutput(sizeof(nonce) + ciphertext.size());
    std::copy(nonce, nonce + sizeof(nonce), finalOutput.begin());
    std::copy(ciphertext.begin(), ciphertext.end(), finalOutput.begin() + sizeof(nonce));

    // Write the encrypted data to the output file
    writeFile(outputFile, finalOutput);
}

void decryptFile(const std::string& pubKeyFile, const std::string& privKeyFile, const std::string& inputFile, const std::string& outputFile) {
    // Read the public key
    std::vector<unsigned char> pubKey = readFile(pubKeyFile);
    if (pubKey.size() != crypto_sign_PUBLICKEYBYTES) {
        throw std::runtime_error("Invalid public key size");
    }

    // Read the private key
    std::vector<unsigned char> privKey = readFile(privKeyFile);
    if (privKey.size() != crypto_sign_SECRETKEYBYTES) {
        throw std::runtime_error("Invalid private key size");
    }

    // Read the input file
    std::vector<unsigned char> ciphertext = readFile(inputFile);

    // Ensure the ciphertext is large enough to contain a nonce and a message
    if (ciphertext.size() < crypto_box_NONCEBYTES + crypto_box_MACBYTES) {
        throw std::runtime_error("Ciphertext too short");
    }

    // Extract the nonce
    unsigned char nonce[crypto_box_NONCEBYTES];
    std::copy(ciphertext.begin(), ciphertext.begin() + crypto_box_NONCEBYTES, nonce);

    // Convert Ed25519 keys to X25519 keys
    auto [x25519_pubkey, x25519_privkey] = convertEd25519KeysToX25519(pubKey, privKey);

    // Prepare the output buffer
    std::vector<unsigned char> plaintext(ciphertext.size() - crypto_box_NONCEBYTES - crypto_box_MACBYTES);

    // Decrypt the message
    if (crypto_box_open_easy(plaintext.data(),
                             ciphertext.data() + crypto_box_NONCEBYTES,
                             ciphertext.size() - crypto_box_NONCEBYTES,
                             nonce,
                             x25519_pubkey.data(),
                             x25519_privkey.data())
        != 0) {
        throw std::runtime_error("Decryption failed");
    }

    // Write the decrypted data to the output file
    writeFile(outputFile, plaintext);
}

int main(int argc, char* argv[]) {
    std::unordered_map<std::string, std::string> args;
    bool encrypt = false;
    bool decrypt = false;
    bool generateKeyPair = false;

    // Parse command line arguments
    for (int i = 1; i < argc; i += 2) {
        std::string arg = argv[i];
        if (arg == "--encrypt") {
            encrypt = true;
            i--;  // No value follows this flag
        } else if (arg == "--decrypt") {
            decrypt = true;
            i--;  // No value follows this flag
        } else if (arg == "--generateKeyPair") {
            generateKeyPair = true;
            i--;  // No value follows this flag
        } else if (i + 1 < argc) {
            args[arg] = argv[i + 1];
        } else {
            std::cerr << "Error: Missing value for argument " << arg << std::endl;
            return 1;
        }
    }

    // Check and generate key pair if needed
    if (generateKeyPair) {
        if (!args.count("--public")) {
            std::cerr << "Error: Must specify --public for encryption" << std::endl;
            return 1;
        }
        if (!args.count("--private")) {
            std::cerr << "Error: Must specify --private for decryption" << std::endl;
            return 1;
        }
    }
    if (generateKeyPair) {
        generateAndSaveKeyPair(args["--public"], args["--private"]);
    }

    // Validate arguments
    if (encrypt == decrypt) {
        std::cerr << "Error: Must specify either --encrypt or --decrypt" << std::endl;
        return 1;
    }
    if (!args.count("--input") || !args.count("--output")) {
        std::cerr << "Error: Must specify both --input and --output" << std::endl;
        return 1;
    }
    if (encrypt) {
        if (!args.count("--public")) {
            std::cerr << "Error: Must specify --public for encryption" << std::endl;
            return 1;
        }
        if (!args.count("--private")) {
            std::cerr << "Error: Must specify --private for decryption" << std::endl;
            return 1;
        }
    }
    if (decrypt) {
        if (!args.count("--public")) {
            std::cerr << "Error: Must specify --public for encryption" << std::endl;
            return 1;
        }
        if (!args.count("--private")) {
            std::cerr << "Error: Must specify --private for decryption" << std::endl;
            return 1;
        }
    }

    if (sodium_init() < 0) {
        std::cerr << "Failed to initialize libsodium" << std::endl;
        return 1;
    }

    try {
        if (encrypt) {
            encryptFile(args["--private"], args["--public"], args["--input"], args["--output"]);
            std::cout << "File encrypted successfully." << std::endl;
        } else {
            decryptFile(args["--public"], args["--private"], args["--input"], args["--output"]);
            std::cout << "File decrypted successfully." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
