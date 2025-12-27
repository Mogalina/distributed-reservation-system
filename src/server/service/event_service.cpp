#include "event_service.hpp"

namespace service {

EventService::EventService(repository::EventRepository& eventRepo,
                           repository::UserRepository& userRepo) 
  : eventRepo_(eventRepo), userRepo_(userRepo) {}

EventListResponse EventService::getEvents(int page, 
                                          int limit, 
                                          const std::string& search, 
                                          const std::string& type) {
  auto result = eventRepo_.findEvents(page, limit, search, type);
  return { result.first, result.second, page, limit };
}

std::optional<models::EventDetails> EventService::getEventDetails(
  const std::string& eventId
) {
  return eventRepo_.findById(eventId);
}

bool EventService::reserveTickets(const std::string& userId, 
                                  const models::ReservationRequest& req) {
  // Fetch user to get national ID
  auto userOpt = userRepo_.read(userId);
  if (!userOpt) {
    return false;
  }

  std::string nationalId = userOpt->nationalId; 
  return eventRepo_.createReservation(userId, nationalId, req);
}

}
