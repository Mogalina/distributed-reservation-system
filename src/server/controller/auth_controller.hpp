#pragma once

#include "controller.hpp"
#include "service/user_service.hpp"
#include "security/jwt.hpp"

namespace controller {

// Controller for handling authentication-related HTTP requests
class AuthController : public RestController {
public:
  AuthController(service::UserService& userService, security::Security& security);

  // Main entry point for handling an HTTP request
  http::HttpResponse handleRequest(const http::HttpRequest& req) override;

private:
  // User service for managing user data
  service::UserService& userService_;
  // Security service for handling JWT tokens
  security::Security& security_;

  // Handler methods for specific auth actions
  http::HttpResponse login(const http::HttpRequest& req);
  // Handler for user registration
  http::HttpResponse registerUser(const http::HttpRequest& req);
  // Handler for user logout
  http::HttpResponse logout(const http::HttpRequest& req);
};

}  // namespace controller
