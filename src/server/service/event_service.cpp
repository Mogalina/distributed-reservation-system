#include "event_service.hpp"

namespace service {

EventService::EventService(repository::EventRepository& repo) : repo_(repo) {}

EventListResponse EventService::getEvents(int page, 
                                          int limit, 
                                          const std::string& search, 
                                          const std::string& type) {
  auto result = repo_.findEvents(page, limit, search, type);
  return { result.first, result.second, page, limit };
}

}