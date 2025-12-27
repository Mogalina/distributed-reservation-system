#pragma once

#include <string>
#include <sstream>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "http/http_types.hpp"
#include "thread_pool.hpp"
#include "controller/auth_controller.hpp"
#include "controller/event_controller.hpp"

namespace server {

class Server {
public:
  Server(const std::string& host, uint16_t port, 
         controller::AuthController& authController,
         controller::EventController& eventController);
  ~Server();

  // Starts the server to listen for incoming connections
  void start();

  // Stops the running server
  void stop();

  // Sends a message to the specified client. The client is identified by its 
  // IP address
  void sendMessage(const std::string& clientIp, const std::string& message);

  // Broadcasts a message to all connected clients
  void broadcastMessage(const std::string& message);

private:
  // Accepts incoming client connections
  void acceptConnections();

  // Handles communication with a connected client
  void handleClient(int clientSocket, std::string clientIp);

  // Helper to parse raw HTTP (very basic implementation)
  http::HttpRequest parseRequest(const std::string& rawData);

  std::string host_;
  uint16_t port_;
  int serverSocket_;

  // ThreadPool for handling client requests concurrently
  ThreadPool threadPool_;

  // References to the controllers
  controller::AuthController& authController_;
  controller::EventController& eventController_;

  // Maps client IP addresses to their socket descriptors
  std::unordered_map<std::string, int> clients_;

  // Mutex to protect access to the `clients_` map
  std::mutex clientsMutex_;

  // Thread for accepting incoming connections
  std::thread acceptThread_;

  // Flag to control the server's running state
  bool isRunning_;
};

}  // namespace server
