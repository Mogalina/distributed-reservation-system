#pragma once

#include <vector>
#include <utility>
#include "models/event.hpp"
#include "event_repository.hpp"
#include "user_repository.hpp"

namespace service {

struct EventListResponse {
  std::vector<models::Event> data;
  int total;
  int page;
  int limit;
};

class EventService {
public:
  explicit EventService(repository::EventRepository& eventRepo,
                        repository::UserRepository& userRepo);

  // Retrieves a list of events with pagination, search, and filtering.
  EventListResponse getEvents(int page, 
                              int limit, 
                              const std::string& search, 
                              const std::string& type);

  // Gets single event with category details                           
  std::optional<models::EventDetails> getEventDetails(
    const std::string& eventId
  );

  // Creates a reservation for a user
  std::string reserveTickets(const std::string& userId, 
                             const models::ReservationRequest& req);

  // Processes the reservation payment                
  bool processPayment(const std::string& reservationId, 
                      const std::string& nationalId, 
                      double amount);

private:
  repository::EventRepository& eventRepo_;
  repository::UserRepository& userRepo_;
};

}