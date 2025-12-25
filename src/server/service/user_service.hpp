#pragma once

#include "service.hpp"
#include "models/user.hpp"
#include "repository/user_repository.hpp"

namespace service {

// Service class for User model
class UserService : public Service<models::User, std::string> {
public:
  explicit UserService(repository::UserRepository& repo);

  models::User registerUser(const std::string& username, 
                            const std::string& password, 
                            const std::string& nationalId);
  std::optional<models::User> authenticate(const std::string& username, 
                                           const std::string& password);

  models::User create(const models::User& user) override;
  std::optional<models::User> getById(const std::string& id) override;
  std::vector<models::User> getAll() override;
  models::User update(const models::User& user) override;
  void deleteById(const std::string& id) override;

private:
  repository::UserRepository& repo_;
};

}  // namespace service