#pragma once

#include <string>
#include <unordered_map>
#include <string_view>

namespace utils {

// Parses an environment file and fills the provided map with key-value pairs
void parseEnvironmentFile(
  std::string_view filePath,
  std::unordered_map<std::string, std::string>& variables
);

// Generates a SHA-256 hash of the input string
std::string generateUUID();

// Hashes a password using SHA-256 and returns the hexadecimal string 
// representation
std::string hashPassword(const std::string& password);

}  // namespace utils
