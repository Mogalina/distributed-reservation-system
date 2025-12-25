#pragma once

#include <string>
#include "security/jwt.hpp"
#include "http/http_types.hpp"

namespace middleware {

// Authentication middleware to verify JWT tokens
http::HttpResponse authMiddleware(const http::HttpRequest& req,
                    security::Security& security, 
                    std::string& userId) {
  // Check for Authorization header                    
  if (!req.headers.contains("Authorization")) {
    return http::HttpResponse::make(400, 
      R"({"error": "Missing Authorization header"})");
  }

  // Extract token from header
  std::string authHeader = req.headers.at("Authorization");
  std::string token;

  // Check Bearer token format
  if (authHeader.find("Bearer ") == 0) {
    token = authHeader.substr(7);
  } else {
    return http::HttpResponse::make(400, 
      R"({"error": "Invalid Authorization header format"})");
  }

  // Verify token
  auto verified = security.verifyToken(token);
  if (!verified) {
    return http::HttpResponse::make(401, 
      R"({"error": "Invalid or expired token"})");
  }

  userId = *verified;
  return http::HttpResponse::make(200, 
    R"({"message": "Authentication successful"})");
}

}  // namespace middleware
