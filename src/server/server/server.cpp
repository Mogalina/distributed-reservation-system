#include "server.hpp"
#include <iostream>
#include <cstring>
#include <sstream>

namespace server {

Server::Server(const std::string& host, uint16_t port, 
               controller::AuthController& authController)
    : host_(host), port_(port), authController_(authController) {
  // Create server socket                                                        
  serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket_ < 0) {
    std::cerr << "Error: Could not create socket." << std::endl;
    throw std::runtime_error("Socket creation failed");
  }

  // Configure server address structure
  sockaddr_in serverAddr{};
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = inet_addr(host.c_str());
  serverAddr.sin_port = htons(port);

  // Set socket options to allow reuse of the address and port
  int opt = 1;
  if (setsockopt(serverSocket_, SOL_SOCKET, SO_REUSEADDR, &opt, 
                 sizeof(opt)) < 0) {
    std::cerr << "Error: Could not set socket options." << std::endl;
    close(serverSocket_);
    throw std::runtime_error("Set socket options failed");
  }

  // Bind the socket to the specified host and port
  if (bind(serverSocket_, (struct sockaddr*)&serverAddr, 
           sizeof(serverAddr)) < 0) {
    std::cerr << "Error: Could not bind socket." << std::endl;
    close(serverSocket_);
    throw std::runtime_error("Socket bind failed");
  }

  // Start listening for incoming connections
  if (listen(serverSocket_, 5) < 0) {
    std::cerr << "Error: Could not listen on socket." << std::endl;
    close(serverSocket_);
    throw std::runtime_error("Socket listen failed");
  }
}

Server::~Server() {
  // Stop the server
  isRunning_ = false;
  close(serverSocket_);

  // Close all client connections
  {
    std::lock_guard<std::mutex> lock(clientsMutex_);
    for (auto& client : clients_) {
        close(client.second);
    }
    clients_.clear();
  }

  // Join the accept thread if it's running
  if (acceptThread_.joinable()) {
    acceptThread_.join();
  }
}

void Server::start() {
  acceptThread_ = std::thread(&Server::acceptConnections, this);
  isRunning_ = true;
  std::cout << "Server running on " << host_ << ":" << port_ << std::endl;
  acceptThread_.join();
}

void Server::acceptConnections() {
  while (isRunning_) {
    // Accept incoming client connection
    sockaddr_in clientAddr{};
    socklen_t clientLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket_, (struct sockaddr*)&clientAddr, 
                              &clientLen);
    if (clientSocket < 0) {
      std::cerr << "Error: Could not accept client connection." << std::endl;
      continue;
    }

    // Get client IP address
    std::string clientIp = inet_ntoa(clientAddr.sin_addr);
    {
      // Add client to the clients map
      std::lock_guard<std::mutex> lock(clientsMutex_);
      clients_[clientIp] = clientSocket;
    }

    // Handle client communication in a separate thread
    std::thread(&Server::handleClient, this, clientSocket, clientIp).detach();
    std::cout << "Client connected: " << clientIp << std::endl;
  }
}

http::HttpRequest Server::parseRequest(const std::string& rawData) {
  http::HttpRequest req;
  std::istringstream stream(rawData);
  std::string line;

  // Parse request line
  if (std::getline(stream, line)) {
    std::istringstream lineStream(line);
    std::string methodStr, path;
    lineStream >> methodStr >> path;
    req.method = http::HttpRequest::stringToMethod(methodStr);
    req.path = path;
  }

  // Skip headers for simplicity
  while (std::getline(stream, line) && line != "\r") {
    if (line.empty() || line == "\r") break;
  }

  // Read body
  std::stringstream bodyStream;
  bodyStream << stream.rdbuf();
  req.body = bodyStream.str();
  
  return req;
}

void Server::handleClient(int clientSocket, std::string clientIp) {
  char buffer[1024];

  while (isRunning_) {
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) {
      // Client disconnected or error occurred
      std::cout << "Client disconnected: " << clientIp << std::endl;
      break;
    }
    
    std::string rawRequest(buffer);
    std::cout << std::endl << "Request: " << rawRequest 
              << std::endl << std::endl;

    // Parse and handle request
    auto req = parseRequest(rawRequest);
    auto resp = authController_.handleRequest(req);

    // Format response
    std::stringstream responseStream;
    responseStream << "HTTP/1.1 " << resp.statusCode << " OK\r\n";
    responseStream << "Content-Type: application/json\r\n";
    responseStream << "Content-Length: " << resp.body.size() << "\r\n";
    responseStream << "\r\n";
    responseStream << resp.body;

    std::string responseStr = responseStream.str();
    send(clientSocket, responseStr.c_str(), responseStr.size(), 0);
  }

  // Remove client from the clients map and close the socket
  {
    std::lock_guard<std::mutex> lock(clientsMutex_);
    clients_.erase(clientIp);
  }
  close(clientSocket);
}

void Server::sendMessage(const std::string& clientIp, 
                         const std::string& message) {
  std::lock_guard<std::mutex> lock(clientsMutex_);
  auto it = clients_.find(clientIp);
  if (it != clients_.end()) {
    // Send message to the specified client
    send(it->second, message.c_str(), message.size(), 0);
  } else {
    // Client not found
    std::cerr << "Error: Client " << clientIp << " not found." << std::endl;
  }
}

void Server::broadcastMessage(const std::string& message) {
  std::lock_guard<std::mutex> lock(clientsMutex_);
  for (const auto& client : clients_) {
    send(client.second, message.c_str(), message.size(), 0);
  }
}

}  // namespace server
