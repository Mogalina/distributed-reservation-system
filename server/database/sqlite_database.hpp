#pragma once

#include "database.hpp"
#include <sqlite3.h>

namespace database {

class SqliteDatabase : public Database {
public:
  SqliteDatabase();
  ~SqliteDatabase() override;

  bool open(const std::string& path) override;
  void close() override;

  bool execute(const std::string& sql) override;
  bool query(const std::string& sql, std::vector<Row>& result) override;

private:
  sqlite3* db_;
};

}  // namespace database
