#include <iostream>
#include "utils/utils.hpp"
#include "server/server.hpp"

int main() {
  // Load environment variables
  std::unordered_map<std::string, std::string> variables;
  utils::parseEnvironmentFile(".env", variables);

  // Initialize the server
  server::Server server(variables["SERVER_HOST"], 
                        std::stoi(variables["SERVER_PORT"]));
  // Start listening for incoming connections                      
  server.start();                      

  return 0;
}
