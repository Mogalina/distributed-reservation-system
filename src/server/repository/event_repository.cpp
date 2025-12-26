#include "event_repository.hpp"
#include <iostream>
#include <sstream>

namespace repository {

EventRepository::EventRepository(database::SqliteDatabase& db) : db_(db) {}

std::pair<std::vector<models::Event>, int> EventRepository::findEvents(
  int page, int limit, 
  const std::string& searchTerm, const std::string& filterType
) {
  std::vector<models::Event> events;
  int offset = (page - 1) * limit;

  // Base query construction
  std::stringstream sql;
  sql << "SELECT "
      << "  e.event_id, e.event_name, e.start_time, e.end_time, "
      << "  (SELECT IFNULL(SUM(capacity), 0) FROM event_ticket_categories WHERE event_id = e.event_id) as total_capacity, "
      << "  (SELECT IFNULL(SUM(ticket_count), 0) FROM reservations WHERE event_id = e.event_id AND status != 'CANCELLED') as sold_tickets "
      << "FROM events e "
      << "WHERE 1=1 ";

  // Search filter
  if (!searchTerm.empty()) {
    sql << "AND e.event_name LIKE '%" << searchTerm << "%' ";
  }

  // Type filters
  if (filterType == "EVENING") {
    sql << "AND e.start_time >= '18:00' ";
  }

  // Grouping to aggregate ticket data
  sql << "GROUP BY e.event_id ";

  // Availability filter
  if (filterType == "AVAILABLE") {
    sql << "HAVING (total_capacity - sold_tickets) > 0 ";
  }

  // Pagination
  sql << "LIMIT " << limit << " OFFSET " << offset;

  // Execute data query
  std::vector<database::Row> rows;
  if (db_.query(sql.str(), rows)) {
    for (const auto& row : rows) {
      models::Event event;
      event.eventId = row.columns[0];
      event.eventName = row.columns[1];
      event.startTime = row.columns[2];
      event.endTime = row.columns[3];
      
      try {
        int totalCapacity = std::stoi(row.columns[4]);
        int soldTickets = std::stoi(row.columns[5]);
        event.capacity = totalCapacity;
        event.availableTickets = std::max(0, totalCapacity - soldTickets);
      } catch (...) {
        event.capacity = 0;
        event.availableTickets = 0;
      }

      events.push_back(event);
    }
  }

  // Calculate total count for pagination
  std::stringstream countSql;
  countSql << "SELECT COUNT(*) FROM (SELECT e.event_id, "
            << "(SELECT IFNULL(SUM(capacity), 0) FROM event_ticket_categories WHERE event_id = e.event_id) as cap, "
            << "(SELECT IFNULL(SUM(ticket_count), 0) FROM reservations WHERE event_id = e.event_id AND status != 'CANCELLED') as sold "
            << "FROM events e WHERE 1=1 ";
  
  if (!searchTerm.empty()) {
    countSql << "AND e.event_name LIKE '%" << searchTerm << "%' ";
  }
  if (filterType == "EVENING") {
    countSql << "AND e.start_time >= '18:00' ";
  }
  
  countSql << "GROUP BY e.event_id ";
  if (filterType == "AVAILABLE") {
    countSql << "HAVING (cap - sold) > 0 ";
  }
  countSql << ")";

  int totalCount = 0;
  std::vector<database::Row> countRows;
  if (db_.query(countSql.str(), countRows) && !countRows.empty()) {
    try {
      totalCount = std::stoi(countRows[0].columns[0]);
    } catch (...) { 
      totalCount = 0; 
    }
  }

  return {events, totalCount};
}

}