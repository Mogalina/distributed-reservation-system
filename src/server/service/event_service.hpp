#pragma once

#include <vector>
#include <utility>
#include "models/event.hpp"
#include "repository/event_repository.hpp"

namespace service {

struct EventListResponse {
  std::vector<models::Event> data;
  int total;
  int page;
  int limit;
};

class EventService {
public:
  explicit EventService(repository::EventRepository& repo);

  // Retrieves a list of events with pagination, search, and filtering.
  EventListResponse getEvents(int page, 
                              int limit, 
                              const std::string& search, 
                              const std::string& type);

private:
  repository::EventRepository& repo_;
};

}