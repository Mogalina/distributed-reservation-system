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
  
  // Finds events with pagination, search term, and optional filter type
  std::pair<std::vector<models::Event>, int> findEvents(
    int page, 
    int limit, 
    const std::string& searchTerm, 
    const std::string& filterType = ""
  );

  // Gets single event with category details
  std::optional<models::EventDetails> findById(const std::string& eventId);

  // Creates a reservation for a user
  bool createReservation(const std::string& userId, 
                         const std::string& nationalId, 
                         const models::ReservationRequest& req);

  // Confirms a reservation payment and records the transaction                    
  bool confirmPayment(const std::string& reservationId, 
                      const std::string& cnp,
                      double amount);

                
  // Cancels all expired reservations                    
  int cancelExpiredReservations(int timeoutSeconds);

  // Generates a stock report for event ticket availability
  std::string generateStockReport();

private:
  database::SqliteDatabase& db_;
};

}
