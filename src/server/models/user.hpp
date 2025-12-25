#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace models {

struct User {
  std::string userId;
  std::string username;
  std::string passwordHash;
  std::string nationalId;
};

// Serialization macro
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
  User, 
  userId, 
  username, 
  passwordHash, 
  nationalId
)

}  // namespace models
