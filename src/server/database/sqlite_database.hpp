#pragma once

#include "database.hpp"
#include <sqlite3.h>
#include <mutex>

namespace database {

// SQLite implementation of the Database interface
class SqliteDatabase : public Database {
public:
  SqliteDatabase();
  ~SqliteDatabase() override;

  bool open(const std::string& path) override;
  void close() override;

  bool execute(const std::string& sql) override;
  bool query(const std::string& sql, std::vector<Row>& result) override;

private:
  // SQLite database connection handle
  sqlite3* db_;

  // Mutex to protect concurrent access to database
  std::mutex dbMutex_;
};

}  // namespace database
