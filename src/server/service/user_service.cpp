#include "user_service.hpp"
#include "utils/utils.hpp"
#include <stdexcept>

namespace service {

UserService::UserService(repository::UserRepository& repo) : repo_(repo) {}

models::User UserService::registerUser(const std::string& username, 
                                       const std::string& password, 
                                       const std::string& nationalId) {
    models::User user;
    user.userId = utils::generateUUID(); 
    user.username = username;
    user.passwordHash = utils::hashPassword(password);
    user.nationalId = nationalId;

    return repo_.create(user);
}

std::optional<models::User>
UserService::authenticate(const std::string& username, 
                          const std::string& password) {
    auto userOpt = repo_.findByUsername(username);
    if (!userOpt) {
      return std::nullopt;
    }

    std::string hashedInput = utils::hashPassword(password);
    if (userOpt->passwordHash == hashedInput) {
        return userOpt;
    }
    return std::nullopt;
}

models::User UserService::create(const models::User& user) {
    if (user.nationalId.empty()) {
        throw std::invalid_argument("National ID cannot be empty");
    }
    return repo_.create(user);
}

std::optional<models::User> UserService::getById(const std::string& id) {
    return repo_.read(id);
}

std::vector<models::User> UserService::getAll() {
    return repo_.readAll();
}

models::User UserService::update(const models::User& user) {
    return repo_.update(user);
}

void UserService::deleteById(const std::string& id) {
    repo_.deleteItem(id);
}

}  // namespace service
