#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <thread>
#include <future>
#include <chrono>
#include <random>
#include <sstream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <mutex> 

// Global log file and mutex
std::ofstream logFile;
std::mutex logMutex;

// Thread-safe logging helper
void logOutput(const std::string& message) {
  std::lock_guard<std::mutex> lock(logMutex);
  
  // Write to standard output
  std::cout << message;
  
  // Write to file if open
  if (logFile.is_open()) {
    logFile << message;
    // Ensure write immediately
    logFile.flush();
  }
}

// Simple HTTP response structure
struct HttpResponse {
  int statusCode;
  std::string body;
};

// Helper function to perform a raw HTTP POST request
HttpResponse sendHttpRequest(const std::string& host, int port, 
                             const std::string& path, 
                             const std::string& jsonBody) {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    return {500, "Socket creation failed"};
  }

  sockaddr_in serverAddr{};
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);
  inet_pton(AF_INET, host.c_str(), &serverAddr.sin_addr);

  if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
    close(sock);
    return {503, "Connection failed"};
  }

  // Construct HTTP request
  std::stringstream req;
  req << "POST " << path << " HTTP/1.1\r\n"
      << "Host: " << host << "\r\n"
      << "Content-Type: application/json\r\n"
      << "Content-Length: " << jsonBody.length() << "\r\n"
      << "Connection: close\r\n"
      << "\r\n"
      << jsonBody;

  std::string requestStr = req.str();
  send(sock, requestStr.c_str(), requestStr.length(), 0);

  // Read response
  std::string responseStr;
  char buffer[4096];
  while (true) {
    ssize_t bytesRead = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) break;
    buffer[bytesRead] = '\0';
    responseStr += buffer;
  }
  close(sock);

  // Basic parsing for status code
  HttpResponse res{0, ""};
  size_t headerEnd = responseStr.find("\r\n\r\n");
  if (headerEnd != std::string::npos) {
    res.body = responseStr.substr(headerEnd + 4);
    size_t firstSpace = responseStr.find(' ');
    if (firstSpace != std::string::npos) {
      try {
        res.statusCode = std::stoi(responseStr.substr(firstSpace + 1, 3));
      } catch (...) {}
    }
  }
  return res;
}

// Function simulating a single client's behavior
void runClient(int clientId) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> eventDist(1, 3);
  
  for (int i = 0; i < 5; ++i) {
    std::string eventId = "a1000000-0000-4000-a000-00000000000" + 
                          std::to_string(eventDist(gen));
    std::string categoryId = "550e8400-e29b-41d4-a716-446655440000"; 
    
    // JSON body for reservation
    std::string json = R"({"eventId":")" + eventId + 
                       R"(","categoryId":")" + categoryId + 
                       R"(","ticketCount":1})";

    {
      std::stringstream ss;
      ss << "[Client " << clientId << "] Requesting reservation for " 
         << eventId << "...\n";
      logOutput(ss.str());
    }

    // We launch the network request asynchronously
    std::future<HttpResponse> responseFuture 
      = std::async(std::launch::async, [json]() {
      // This runs on a separate thread, managed by the system
      return sendHttpRequest("127.0.0.1", 8080, "/events/reserve", json);
    });

    // While the request is processing, the main client thread is free
    // Blocks until the future is ready
    HttpResponse res = responseFuture.get();

    std::stringstream ss;
    if (res.statusCode == 201) {
      ss << "[Client " << clientId 
         << "] SUCCESS: " << res.body << "\n";
    } else {
      ss << "[Client " << clientId 
         << "] FAILED (" << res.statusCode << "): " << res.body << "\n";
    }
    logOutput(ss.str());

    // Wait seconds before next request
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }
}

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Invalid number of arguments.\n";
    return EXIT_FAILURE;
  }

  // Extract number of concurrent clients
  const int numClients = std::stoi(argv[1]);

  // Ensure logs directory exists
  std::filesystem::create_directories("logs");

  logFile.open("logs/simulation.log", std::ios::out | std::ios::app);
  if (!logFile.is_open()) {
    std::cerr << "Warning: Could not open logs/simulation.log file.\n";
  }

  std::cout << "Starting client simulation...\n";

  // Launch concurrent clients
  std::vector<std::thread> clients;
  for (int i = 1; i <= numClients; ++i) {
    clients.emplace_back(runClient, i);
  }

  // Wait for all clients to finish
  for (auto& client : clients) {
    client.join();
  }

  std::cout << "Simulation completed successfully.\n";
  
  if (logFile.is_open()) {
    logFile.close();
  }
  
  return 0;
}
