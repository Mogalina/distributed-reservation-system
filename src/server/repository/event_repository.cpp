#include "event_repository.hpp"
#include "utils/utils.hpp"
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
      << "  e.event_id, "
      << "  e.event_name, "
      << "  e.start_time, "
      << "  e.end_time, "
      << "  (SELECT IFNULL(SUM(capacity), 0) "
      << "    FROM event_ticket_categories "
      << "    WHERE event_id = e.event_id) AS total_capacity, "
      << "  (SELECT IFNULL(SUM(ticket_count), 0) "
      << "    FROM reservations "
      << "    WHERE event_id = e.event_id AND status != 'CANCELLED') "
      << "    AS sold_tickets "
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
           << "(SELECT IFNULL(SUM(capacity), 0) "
           << "  FROM event_ticket_categories "
           << "  WHERE event_id = e.event_id) as cap, "
           << "(SELECT IFNULL(SUM(ticket_count), 0) "
           << "  FROM reservations "
           << "  WHERE event_id = e.event_id AND status != 'CANCELLED') "
           << "  AS sold "
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
  countSql << ") sub";

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

std::optional<models::EventDetails> EventRepository::findById(
  const std::string& eventId
) {
  // Fetch event basic information
  std::string eventSql = "SELECT event_name, start_time, end_time "
    "FROM events WHERE event_id = '" + eventId + "'";

  std::vector<database::Row> eventRows;
  if (!db_.query(eventSql, eventRows) || eventRows.empty()) {
    return std::nullopt;
  }

  models::EventDetails details;
  details.eventId = eventId;
  details.eventName = eventRows[0].columns[0];
  details.startTime = eventRows[0].columns[1];
  details.endTime = eventRows[0].columns[2];

  // Fetch categories and availability
  std::string catSql = R"(
    SELECT 
      c.category_id, 
      c.name, 
      etc.price, 
      etc.capacity,
      (SELECT COUNT(*) FROM reservations r 
        WHERE r.event_id = etc.event_id 
          AND r.category_id = etc.category_id 
          AND r.status != 'CANCELLED') as sold
    FROM event_ticket_categories etc
    JOIN ticket_categories c ON etc.category_id = c.category_id
    WHERE etc.event_id = ')" + eventId + "'";

  std::vector<database::Row> catRows;
  if (db_.query(catSql, catRows)) {
    for (const auto& row : catRows) {
      models::EventCategory cat;
      cat.categoryId = row.columns[0];
      cat.name = row.columns[1];

      try {
        cat.price = std::stod(row.columns[2]);
        cat.capacity = std::stoi(row.columns[3]);
        int sold = std::stoi(row.columns[4]);
        cat.available = std::max(0, cat.capacity - sold);
      } catch (...) {
        continue; 
      }

      details.categories.push_back(cat);
    }
  }

  return details;
}

bool EventRepository::createReservation(const std::string& userId, 
                                        const std::string& nationalId, 
                                        const models::ReservationRequest& req) {
  // Check availability and get price
  std::string checkSql = R"(
    SELECT 
      etc.capacity,
      (SELECT COUNT(*) FROM reservations r 
        WHERE r.event_id = etc.event_id 
          AND r.category_id = etc.category_id 
          AND r.status != 'CANCELLED') as sold,
      etc.price
    FROM event_ticket_categories etc
    WHERE etc.event_id = ')" + req.eventId + 
    "' AND etc.category_id = '" + req.categoryId + "'";

  std::vector<database::Row> rows;
  if (!db_.query(checkSql, rows) || rows.empty()) {
    return false;
  }

  int capacity = 0, sold = 0;
  double price = 0.0;
  try {
    capacity = std::stoi(rows[0].columns[0]);
    sold = std::stoi(rows[0].columns[1]);
    price = std::stod(rows[0].columns[2]);
  } catch (...) { 
    return false; 
  }

  if (sold + req.ticketCount > capacity) {
    return false;
  }

  // Prepare data for insertion
  std::string reservationId = utils::generateUUID();
  double totalPrice = price * req.ticketCount;
  
  // Get current time string
  time_t now = time(0);
  char* dt = ctime(&now);
  std::string timeStr(dt);
  if (!timeStr.empty() && timeStr.back() == '\n') {
    timeStr.pop_back();
  }

  // Insert into reservations table
  std::stringstream insertSql;
  insertSql << "INSERT INTO reservations VALUES ('"
            << reservationId << "', '"
            << userId << "', '"
            << req.eventId << "', '"
            << req.categoryId << "', '"
            << nationalId << "', "
            << req.ticketCount << ", "
            << "'CONFIRMED', "
            << totalPrice << ", '"
            << timeStr << "')";

  return db_.execute(insertSql.str());
}

}
