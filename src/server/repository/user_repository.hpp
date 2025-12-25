#pragma once

#include "repository.hpp"
#include "models/user.hpp"
#include "database/sqlite_database.hpp"
#include <vector>
#include <optional>

namespace repository {

// UserRepository class for managing User entities in the database.
class UserRepository : public Repository<models::User, std::string> {
public:
  explicit UserRepository(database::SqliteDatabase& db);

  models::User create(const models::User& user) override;
  std::optional<models::User> read(const std::string& id) override;
  std::vector<models::User> readAll() override;
  models::User update(const models::User& user) override;
  void deleteItem(const std::string& id) override;

private:
  database::SqliteDatabase& db_;
};

}  // namespace repository
