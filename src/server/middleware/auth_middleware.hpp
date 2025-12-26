#pragma once

#include <string>
#include <algorithm>
#include "security/jwt.hpp"
#include "http/http_types.hpp"

namespace middleware {

namespace {

// Helper function to trim whitespaces
inline std::string trim(const std::string& str) {
  size_t first = str.find_first_not_of(' ');
  if (std::string::npos == first) return str;
  size_t last = str.find_last_not_of(' ');
  return str.substr(first, (last - first + 1));
}

}  // namespace

// Authentication middleware to verify JWT tokens
inline http::HttpResponse authMiddleware(const http::HttpRequest& req,
                                         security::Security& security, 
                                         std::string& userId) {
  // Check for Authorization header                    
  std::string authHeader;
  if (req.headers.count("Authorization")) {
    authHeader = req.headers.at("Authorization");
  } else if (req.headers.count("authorization")) {
    authHeader = req.headers.at("authorization");
  } else {
    return http::HttpResponse::make(400, 
      R"({"error": "Missing Authorization header"})");
  }

  // Extract token from header
  std::string token;
  if (authHeader.find("Bearer ") == 0) {
    token = authHeader.substr(7);
  } else {
    return http::HttpResponse::make(400, 
      R"({"error": "Invalid Authorization header format"})");
  }

  // Trim token
  token = trim(token);
  while (!token.empty() && (token.back() == '\r' || token.back() == '\n')) {
    token.pop_back();
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
