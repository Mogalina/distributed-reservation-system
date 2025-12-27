#include "sqlite_database.hpp"
#include <iostream>

namespace database {

SqliteDatabase::SqliteDatabase() : db_(nullptr) {}

SqliteDatabase::~SqliteDatabase() {
  close();
}

bool SqliteDatabase::open(const std::string& path) {
  std::lock_guard<std::mutex> lock(dbMutex_);
  if (sqlite3_open(path.c_str(), &db_) != SQLITE_OK) {
    std::cerr << "Failed to open database: " << sqlite3_errmsg(db_) 
              << std::endl;
    return false;
  }
  return true;
}

void SqliteDatabase::close() {
  std::lock_guard<std::mutex> lock(dbMutex_);
  if (db_) {
    sqlite3_close(db_);
    db_ = nullptr;
  }
}

bool SqliteDatabase::execute(const std::string& sql) {
  std::lock_guard<std::mutex> lock(dbMutex_);

  if (!db_) {
    return false;
  }

  char* error = nullptr;
  if (sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &error) != SQLITE_OK) {
    std::cerr << "SQL error: " << error << std::endl;
    sqlite3_free(error);
    return false;
  }
  return true;
}

bool SqliteDatabase::query(const std::string& sql, std::vector<Row>& result) {
  std::lock_guard<std::mutex> lock(dbMutex_);
  
  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) 
              << std::endl;
    return false;
  }

  int cols = sqlite3_column_count(stmt);
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    Row row;
    row.columns.reserve(cols);

    for (int i = 0; i < cols; ++i) {
      const char* text = 
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
      row.columns.emplace_back(text ? reinterpret_cast<const char*>(text) : "");
    }

    result.push_back(row);
  }

  sqlite3_finalize(stmt);
  return true;
}

}  // namespace database