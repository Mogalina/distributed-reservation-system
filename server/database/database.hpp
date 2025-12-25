#pragma once

#include <string>
#include <vector>

namespace database {

// A simple representation of a database row. Each row contains a vector of 
// string columns.
struct Row {
  std::vector<std::string> columns;
};

// Abstract base class for database operations. Provides an interface for
// opening, closing, executing commands, and querying the database.
class Database {
public:
  virtual ~Database() = default;

  // Opens a connection to the database at the specified path
  virtual bool open(const std::string& path) = 0;

  // Closes the database connection
  virtual void close() = 0;

  // Executes a non-query SQL command (e.g., INSERT, UPDATE, DELETE)
  virtual bool execute(const std::string& sql) = 0;

  // Executes a query SQL command (e.g., SELECT) and fills the `result` vector
  // with the retrieved rows
  virtual bool query(const std::string& sql, std::vector<Row>& result) = 0;
};

}  // namespace database
