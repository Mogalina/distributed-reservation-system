#include "auth_controller.hpp"
#include "middleware/auth_middleware.hpp"
#include <nlohmann/json.hpp>
#include <iostream>

namespace controller {

namespace {

using middleware::authMiddleware;

}  // namespace

AuthController::AuthController(service::UserService& userService, 
                               security::Security& security)
    : userService_(userService), security_(security) {}

http::HttpResponse AuthController::handleRequest(const http::HttpRequest& req) {
  // Route to appropriate handler based on path and method
  if (req.path.find("/login") != std::string::npos && 
      req.method == http::Method::POST) {
    return login(req);
  }
  if (req.path.find("/register") != std::string::npos && 
      req.method == http::Method::POST) {
    return registerUser(req);
  }
  if (req.path.find("/logout") != std::string::npos && 
      req.method == http::Method::POST) {
    return logout(req);
  }

  return http::HttpResponse::make(404, 
    R"({"error": "Auth endpoint not found"})");
}

http::HttpResponse AuthController::login(const http::HttpRequest& req) {
  try {
    auto json = nlohmann::json::parse(req.body);
    std::string username = json.value("username", "");
    std::string password = json.value("password", "");

    auto user = userService_.authenticate(username, password);
    if (user) {
      std::string token = security_.generateToken(user->userId);
      return http::HttpResponse::make(200, 
        nlohmann::json({{"token", token}, {"userId", user->userId}}).dump());
    }

    return http::HttpResponse::make(401, R"({"error": "Invalid credentials"})");

  } catch (const std::exception& e) {
    return http::HttpResponse::make(500, 
      nlohmann::json({{"error", e.what()}}).dump());
  }
}

http::HttpResponse AuthController::registerUser(const http::HttpRequest& req) {
  try {
    auto json = nlohmann::json::parse(req.body);
    std::string username = json.value("username", "");
    std::string password = json.value("password", "");
    std::string nationalId = json.value("nationalId", "");

    if(username.empty() || password.empty() || nationalId.empty()) {
      return http::HttpResponse::make(400, R"({"error": "Missing fields"})");
    }

    try {
      auto user = userService_.registerUser(username, password, nationalId);
      return http::HttpResponse::make(201, nlohmann::json({
        {"message", "Created"}, 
        {"userId", user.userId}
      }).dump());

    } catch (const std::exception& e) {
      return http::HttpResponse::make(409, 
        nlohmann::json({{"error", e.what()}}).dump());
    }

  } catch (const std::exception& e) {
    return http::HttpResponse::make(500, 
      nlohmann::json({{"error", e.what()}}).dump());
  }
}

http::HttpResponse AuthController::logout(const http::HttpRequest& req) {
  std::string userId;
  if (authMiddleware(req, security_, userId).statusCode != 200) {
    return http::HttpResponse::make(401, 
      R"({"error": "Unauthorized"})");
  }

  // For JWT, logout is typically handled client-side by discarding the token
  return http::HttpResponse::make(200, 
    R"({"message": "Logged out successfully"})");
}

}  // namespace controller
