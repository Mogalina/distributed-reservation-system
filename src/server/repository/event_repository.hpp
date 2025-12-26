#pragma once

#include <vector>
#include <string>
#include <utility>
#include "models/event.hpp"
#include "database/sqlite_database.hpp"

namespace repository {

class EventRepository {
public:
  explicit EventRepository(database::SqliteDatabase& db);
  
  // Finds events with pagination, search term, and optional filter type.
  std::pair<std::vector<models::Event>, int> findEvents(
    int page, 
    int limit, 
    const std::string& searchTerm, 
    const std::string& filterType = ""
  );

private:
  database::SqliteDatabase& db_;
};

}
