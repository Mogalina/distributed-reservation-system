#pragma once

#include "controller.hpp"
#include "service/event_service.hpp"
#include "security/jwt.hpp"
#include <nlohmann/json.hpp>

namespace controller {

// Controller class for Event-related HTTP requests
class EventController : public RestController {
public:
  explicit EventController(service::EventService& service, 
                           security::Security& security);

  // Main entry point for handling an HTTP request
  http::HttpResponse handleRequest(const http::HttpRequest& req) override;

private:
  service::EventService& service_;
  
  // Security service for handling JWT tokens
  security::Security& security_;
  
  http::HttpResponse handleGet(const http::HttpRequest& req);

  http::HttpResponse handleGetById(const http::HttpRequest& req, 
                                   const std::string& id);

  http::HttpResponse handleReserve(const http::HttpRequest& req);

  http::HttpResponse handlePayment(const http::HttpRequest& req);
};

}  // namespace controller