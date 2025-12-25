#pragma once

#include <jwt-cpp/jwt.h>
#include <string>
#include <chrono>

namespace security {

// Security class for handling JWT token generation and verification
class Security {
public:
  explicit Security(std::string secret) : secret_(std::move(secret)) {}

  // Generate a JWT token for a given user ID with an optional expiration time
  std::string generateToken(const std::string& userId, 
                            const int expireMinutes = 60);

  // Verify a JWT token and return the user ID if valid
  std::optional<std::string> verifyToken(const std::string& token);

private:
  // Secret key for signing the JWT tokens
  std::string secret_;
};

}  // namespace security
