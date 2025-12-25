#include "user_repository.hpp"
#include <stdexcept>
#include <sstream>

namespace repository {

UserRepository::UserRepository(database::SqliteDatabase& db) : db_(db) {}

models::User UserRepository::create(const models::User& user) {
  std::stringstream sql;
  sql << "INSERT INTO users (user_id, first_name, last_name, national_id) "
         "VALUES ('" << user.userId << "','" << user.firstName << "','"
      << user.lastName << "','" << user.nationalId << "');";

  db_.execute(sql.str());
  return user;
}

std::optional<models::User> UserRepository::read(const std::string& id) {
  std::vector<database::Row> rows;
  std::stringstream sql;
  sql << "SELECT user_id, first_name, last_name, national_id "
         "FROM users WHERE user_id='" << id << "';";

  if (!db_.query(sql.str(), rows) || rows.empty()) {
    return std::nullopt;
  }

  const auto& r = rows[0];
  return models::User{r.columns[0], r.columns[1], r.columns[2], r.columns[3]};
}

std::vector<models::User> UserRepository::readAll() {
  std::vector<database::Row> rows;
  db_.query("SELECT user_id, first_name, last_name, national_id FROM users;",
    rows);

  std::vector<models::User> result;
  for (const auto& r : rows) {
    result.push_back(models::User{
      r.columns[0], 
      r.columns[1], 
      r.columns[2], 
      r.columns[3]}
    );
  }
  return result;
}

models::User UserRepository::update(const models::User& user) {
  std::stringstream sql;
  sql << "UPDATE users SET first_name='" << user.firstName
      << "', last_name='" << user.lastName
      << "', national_id='" << user.nationalId
      << "' WHERE user_id='" << user.userId << "';";

  db_.execute(sql.str());
  return user;
}

void UserRepository::deleteItem(const std::string& id) {
  std::stringstream sql;
  sql << "DELETE FROM users WHERE user_id='" << id << "';";

  db_.execute(sql.str());
}

}  // namespace repository
