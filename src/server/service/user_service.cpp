#include "user_service.hpp"
#include <stdexcept>

namespace service {

UserService::UserService(repository::UserRepository& repo) : repo_(repo) {}

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
