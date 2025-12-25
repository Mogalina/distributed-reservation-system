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
    auto jsonBody = nlohmann::json::parse(req.body);
    
    // Extract user identifier
    if (!jsonBody.contains("userId")) {
      return http::HttpResponse::make(400, 
        R"({"error": "Missing user identifier"})");
    }
    std::string userId = jsonBody["userId"];

    // Verify credentials
    auto userOpt = userService_.getById(userId);
    if (!userOpt) {
      return http::HttpResponse::make(401, 
        R"({"error": "Invalid credentials"})");
    }

    // Generate Token
    std::string token = security_.generateToken(userId);
    
    nlohmann::json response;
    response["token"] = token;
    response["userId"] = userId;
    
    return http::HttpResponse::make(200, response.dump());

  } catch (const std::exception& e) {
    return http::HttpResponse::make(500, 
      nlohmann::json({{"error", e.what()}}).dump());
  }
}

http::HttpResponse AuthController::registerUser(const http::HttpRequest& req) {
  try {
    auto jsonBody = nlohmann::json::parse(req.body);
    
    // Manual mapping or use the macro from previous context if available
    models::User user;
    user.userId = jsonBody.value("userId", "");
    user.firstName = jsonBody.value("firstName", "");
    user.lastName = jsonBody.value("lastName", "");
    user.nationalId = jsonBody.value("nationalId", "");

    if (user.userId.empty() || user.nationalId.empty()) {
        return http::HttpResponse::make(400, 
          R"({"error": "Missing required fields"})");
    }

    // Check if user already exists
    if (userService_.getById(user.userId)) {
      return http::HttpResponse::make(409,
        R"({"error": "User already exists"})");
    }

    userService_.create(user);
    
    // Auto-login after register
    std::string token = security_.generateToken(user.userId);
    
    nlohmann::json response;
    response["message"] = "User created successfully";
    response["token"] = token;

    return http::HttpResponse::make(201, response.dump());

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
