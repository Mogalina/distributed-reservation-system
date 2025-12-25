#pragma once

#include <string>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

namespace server {

class Server {
public:
  Server(const std::string& host, uint16_t port);
  ~Server();

  // Starts the server to listen for incoming connections.
  void start();

  // Sends a message to the specified client. The client is identified by its 
  // IP address.
  void sendMessage(const std::string& clientIp, const std::string& message);

  // Broadcasts a message to all connected clients.
  void broadcastMessage(const std::string& message);

private:
  // Accepts incoming client connections.
  void acceptConnections();

  // Handles communication with a connected client.
  void handleClient(int clientSocket, std::string clientIp);

  std::string host_;
  uint16_t port_;
  int serverSocket_;

  // Maps client IP addresses to their socket descriptors.
  std::unordered_map<std::string, int> clients_;

  // Mutex to protect access to the `clients_` map.
  std::mutex clientsMutex_;

  // Thread for accepting incoming connections.
  std::thread acceptThread_;

  // Flag to control the server's running state.
  bool isRunning_;
};

}  // namespace server
