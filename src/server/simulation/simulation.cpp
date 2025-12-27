#include <iostream>
#include <fstream>
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

// Global settings
std::ofstream logFile;
std::mutex logMutex;
int SERVER_PORT = 8080;

struct UserCredentials {
  std::string username;
  std::string password;
};

void logOutput(const std::string& message) {
  std::lock_guard<std::mutex> lock(logMutex);
  std::cout << message;
  if (logFile.is_open()) {
    logFile << message;
    logFile.flush();
  }
}

std::vector<UserCredentials> loadUsers(const std::string& filename) {
  std::vector<UserCredentials> users;
  std::ifstream file(filename);
  if (!file.is_open()) return users;
  
  std::string line;
  while (std::getline(file, line)) {
    if (line.empty()) continue;
    std::stringstream ss(line);
    std::string u, p;
    if (ss >> u >> p) users.push_back({u, p});
  }
  return users;
}

struct HttpResponse {
  int statusCode;
  std::string body;
};

std::string extractJsonValue(const std::string& body, const std::string& key) {
  std::string searchKey = "\"" + key + "\":\"";
  size_t start = body.find(searchKey);
  if (start == std::string::npos) return "";
  start += searchKey.length();
  size_t end = body.find("\"", start);
  if (end == std::string::npos) return "";
  return body.substr(start, end - start);
}

HttpResponse sendHttpRequest(const std::string& host, int port, 
                             const std::string& path, 
                             const std::string& jsonBody,
                             const std::string& authToken = "") {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) return {500, "Socket creation failed"};

  sockaddr_in serverAddr{};
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);
  inet_pton(AF_INET, host.c_str(), &serverAddr.sin_addr);

  if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
    close(sock);
    return {503, "Connection failed"};
  }

  std::stringstream req;
  req << "POST " << path << " HTTP/1.1\r\n"
      << "Host: " << host << "\r\n"
      << "Content-Type: application/json\r\n";
  if (!authToken.empty()) req << "Authorization: Bearer " << authToken << "\r\n";
  
  req << "Content-Length: " << jsonBody.length() << "\r\n"
      << "Connection: close\r\n"
      << "\r\n"
      << jsonBody;

  std::string requestStr = req.str();
  send(sock, requestStr.c_str(), requestStr.length(), 0);

  std::string responseStr;
  char buffer[4096];
  int contentLength = -1;
  int headerLen = -1;

  while (true) {
    ssize_t bytesRead = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) break;
    buffer[bytesRead] = '\0';
    responseStr += buffer;

    if (headerLen == -1) {
      size_t headerEnd = responseStr.find("\r\n\r\n");
      if (headerEnd != std::string::npos) {
        headerLen = headerEnd + 4;
        size_t clPos = responseStr.find("Content-Length: ");
        if (clPos != std::string::npos) {
          size_t endLine = responseStr.find("\r", clPos);
          if (endLine != std::string::npos) {
            try {
                std::string val 
                  = responseStr.substr(clPos + 16, endLine - (clPos + 16));
                contentLength = std::stoi(val);
            } catch (...) { contentLength = 0; }
          }
        } else { 
          contentLength = 0; 
        }
      }
    }
    if (headerLen != -1 && contentLength != -1) {
      if (responseStr.size() >= (size_t)(headerLen + contentLength)) break;
    }
  }
  close(sock);

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

void runClient(int clientId, UserCredentials creds) {
  std::string username = creds.username;
  std::string password = creds.password;
  std::string nationalId = "NID_" + username + "_" + std::to_string(clientId);

  // Register
  std::string regJson = R"({"username":")" + username + 
                        R"(","password":")" + password + 
                        R"(","nationalId":")" + nationalId + R"("})";
  sendHttpRequest("127.0.0.1", SERVER_PORT, "/auth/register", regJson);

  // Login
  std::string loginJson = R"({"username":")" + username + 
                          R"(","password":")" + password + R"("})";
  HttpResponse loginRes = sendHttpRequest("127.0.0.1", SERVER_PORT, 
                                          "/auth/login", loginJson);
  std::string token = extractJsonValue(loginRes.body, "token");

  if (token.empty()) {
    std::stringstream ss;
    ss << "[Client " << username << "] Auth Failed.\n";
    logOutput(ss.str());
    return;
  }

  // Start actions
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> eventDist(1, 3);
  
  for (int i = 0; i < 5; ++i) {
    std::string eventId = "a1000000-0000-4000-a000-00000000000" + 
                          std::to_string(eventDist(gen));
    std::string categoryId = "550e8400-e29b-41d4-a716-446655440000"; 
    std::string json = R"({"eventId":")" + eventId + 
                       R"(","categoryId":")" + categoryId + 
                       R"(","ticketCount":1})";

    {
      std::stringstream ss;
      ss << "[Client " << username << "] Requesting " << eventId << "...\n";
      logOutput(ss.str());
    }

    auto responseFuture = std::async(std::launch::async, [json, token]() {
      return sendHttpRequest("127.0.0.1", SERVER_PORT, 
                             "/events/reserve", json, token);
    });

    HttpResponse res = responseFuture.get();

    if (res.statusCode == 201) {
      std::string resId = extractJsonValue(res.body, "reservationId");
      
      {
        std::stringstream ss;
        ss << "[Client " << username << "] Reserved ID: " 
           << resId << ". Paying...\n";
        logOutput(ss.str());
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(500));

      std::string payJson = R"({"reservationId":")" + resId + 
                            R"(","nationalId":")" + nationalId + 
                            R"(","amount":100.0})"; 
      
      auto payFuture = std::async(std::launch::async, [payJson, token]() {
        return sendHttpRequest("127.0.0.1", SERVER_PORT, 
                               "/events/pay", payJson, token);
      });
      
      HttpResponse payRes = payFuture.get();
      std::stringstream ss;
      if (payRes.statusCode == 200) {
        ss << "[Client " << username << "] Payment SUCCESS.\n";
      } else {
        ss << "[Client " << username << "] Payment FAILED (" 
           << payRes.statusCode << "): " << payRes.body << "\n";
      }
      logOutput(ss.str());

    } else {
      std::stringstream ss;
      ss << "[Client " << username << "] Reservation FAILED: " 
         << res.body << "\n";
      logOutput(ss.str());
  }
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: ./simulation <config_file> [port]\n";
    return 1;
  }

  if (argc > 2) {
    SERVER_PORT = std::stoi(argv[2]);
  }

  std::vector<UserCredentials> users = loadUsers(argv[1]);
  if (users.empty()) {
    std::cerr << "Error: No users found in " << argv[1] << "\n";
    return 1;
  }

  logFile.open("logs/simulation.log", std::ios::out | std::ios::trunc);
  std::cout << "Starting simulation...\n";

  std::vector<std::thread> clients;
  for (size_t i = 0; i < users.size(); ++i) {
    clients.emplace_back(runClient, i + 1, users[i]);
  }

  for (auto& client : clients) client.join();

  std::cout << "Simulation completed successfully.\n";
  if (logFile.is_open()) {
    logFile.close();
  }

  return 0;
}
