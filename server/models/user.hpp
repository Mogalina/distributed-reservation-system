#pragma once

#include <string>

namespace models {

struct User {
  std::string userId;
  std::string firstName;
  std::string lastName;
  std::string nationalId;
};

}  // namespace models
