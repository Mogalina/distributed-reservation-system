#include "utils.hpp"
#include <string>
#include <unordered_map>
#include <random>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <openssl/sha.h>

namespace utils {

// Parses an environment file and fills the provided map with key-value pairs
void parseEnvironmentFile(
  std::string_view filePath,
  std::unordered_map<std::string, std::string>& variables
) {
    variables.clear();

    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open environment file: "
                  << filePath << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        size_t delimiterPos = line.find('=');
        if (delimiterPos == std::string::npos) continue;

        variables[line.substr(0, delimiterPos)] = line.substr(delimiterPos + 1);
    }
}

// Generates a SHA-256 hash of the input string
std::string generateUUID() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static std::uniform_int_distribution<> dis2(8, 11);

    std::stringstream ss;
    int i;
    ss << std::hex;
    for (i = 0; i < 8; i++) ss << dis(gen);
    ss << "-";
    for (i = 0; i < 4; i++) ss << dis(gen);
    ss << "-4";
    for (i = 0; i < 3; i++) ss << dis(gen);
    ss << "-";
    ss << dis2(gen);
    for (i = 0; i < 3; i++) ss << dis(gen);
    ss << "-";
    for (i = 0; i < 12; i++) ss << dis(gen);
    return ss.str();
}

// Hashes a password using SHA-256 and returns the hexadecimal string 
// representation
std::string hashPassword(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(password.c_str()), 
                                                  password.size(), 
                                                  hash);
    
    std::stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

}  // namespace utils
