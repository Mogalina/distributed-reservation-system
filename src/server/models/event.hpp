#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace models {

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

}