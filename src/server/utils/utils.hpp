#pragma once

#include <fstream>
#include <iostream>
#include <unordered_map>

namespace utils {

// Parses an environment file and fills the provided map with key-value pairs.
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

}  // namespace utils
