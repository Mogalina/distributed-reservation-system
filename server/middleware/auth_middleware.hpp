#pragma once

#include <string>
#include "security/jwt.hpp"

namespace middleware {

// Authentication middleware to verify JWT tokens
bool authMiddleware(const std::string& token, 
                    Security& security, 
                    std::string& userId) {
    auto verified = security.verifyToken(token);
    if (!verified) {
      return false;
    }
    
    userId = *verified;
    return true;
}

}  // namespace middleware
