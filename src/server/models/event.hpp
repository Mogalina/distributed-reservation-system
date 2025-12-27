#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace models {

// Basic event
struct Event {
  std::string eventId;
  std::string eventName;
  std::string startTime;
  std::string endTime;
  int capacity;
  int availableTickets;
};

// Serialization macro
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
  Event, 
  eventId, 
  eventName, 
  startTime, 
  endTime, 
  capacity, 
  availableTickets
)

// Category details for a specific event
struct EventCategory {
  std::string categoryId;
  std::string name;
  double price;
  int capacity;
  int available;
};

// Serialization macro
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
  EventCategory, 
  categoryId, 
  name, 
  price, 
  capacity, 
  available
)

// Full event details
struct EventDetails {
  std::string eventId;
  std::string eventName;
  std::string startTime;
  std::string endTime;
  std::vector<EventCategory> categories;
};

// Serialization macro
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
  EventDetails, 
  eventId, 
  eventName, 
  startTime, 
  endTime, 
  categories
)

// Reservation Request
struct ReservationRequest {
  std::string eventId;
  std::string categoryId;
  int ticketCount;
};

// Serialization macro
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
  ReservationRequest, 
  eventId, 
  categoryId, 
  ticketCount
)

}