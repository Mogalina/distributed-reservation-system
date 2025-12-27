#include "event_controller.hpp"
#include "middleware/auth_middleware.hpp"
#include <nlohmann/json.hpp>
#include <string>

namespace controller {

namespace {

using middleware::authMiddleware;

}  // namespace

EventController::EventController(service::EventService& service, 
                                 security::Security& security) 
  : service_(service), security_(security) {}

http::HttpResponse EventController::handleRequest(
  const http::HttpRequest& req
) {
  // Handle ticket reservation
  if (req.method == http::Method::POST && req.path == "/events/reserve") {
    return handleReserve(req);
  }

  // Handle ticket payment
  if (req.method == http::Method::POST && req.path == "/events/pay") {
    return handlePayment(req);
  }

  if (req.method == http::Method::GET) {
    // Check for specific ID
    std::string prefix = "/events/";
    if (req.path.rfind(prefix, 0) == 0 && req.path.length() > prefix.length()) {
      std::string id = req.path.substr(prefix.length());
      // Clean ID from query parameters
      size_t qPos = id.find('?');
      if (qPos != std::string::npos) {
        id = id.substr(0, qPos);
      }
      
      return handleGetById(req, id);
    }
    
    // Default to list
    return handleGet(req);
  }

  return http::HttpResponse::make(405, "Method Not Allowed");
}

http::HttpResponse EventController::handleGet(const http::HttpRequest& req) {
  std::string userId;
  if (authMiddleware(req, security_, userId).statusCode != 200) {
    return http::HttpResponse::make(401, R"({"error": "Unauthorized"})");
  }

  // Default parameters
  int page = 1;
  int limit = 5;
  std::string search = "";
  std::string type = "ALL";

  // Parse query parameters
  if (req.queryParams.count("page")) {
    try { 
      page = std::stoi(req.queryParams.at("page")); 
    } catch (...) {}
  }
  if (req.queryParams.count("limit")) {
    try { 
      limit = std::stoi(req.queryParams.at("limit")); 
    } catch (...) {}
  }
  if (req.queryParams.count("search")) {
    search = req.queryParams.at("search");
  }
  if (req.queryParams.count("type")) {
    type = req.queryParams.at("type");
  }

  auto result = service_.getEvents(page, limit, search, type);

  // Construct JSON response
  nlohmann::json response;
  response["data"] = result.data;
  response["meta"] = {
    {"total", result.total},
    {"page", result.page},
    {"limit", result.limit},
    {"totalPages", (result.total + result.limit - 1) / result.limit}
  };

  return http::HttpResponse::make(200, response.dump());
}

http::HttpResponse EventController::handleGetById(const http::HttpRequest& req, 
                                                  const std::string& id) {
  std::string userId;
  if (authMiddleware(req, security_, userId).statusCode != 200) {
    return http::HttpResponse::make(401, R"({"error": "Unauthorized"})");
  }

  auto details = service_.getEventDetails(id);
  if (details) {
    nlohmann::json json = *details;
    return http::HttpResponse::make(200, json.dump());
  }
  return http::HttpResponse::make(404, R"({"error": "Event not found"})");
}

http::HttpResponse EventController::handleReserve(
  const http::HttpRequest& req
) {
  std::string userId;
  if (authMiddleware(req, security_, userId).statusCode != 200) {
    return http::HttpResponse::make(401, R"({"error": "Unauthorized"})");
  }

  try {
    auto json = nlohmann::json::parse(req.body);
    models::ReservationRequest resReq = json.get<models::ReservationRequest>();
    
    // Retrieve reservation ID
    std::string resId = service_.reserveTickets(userId, resReq);

    if (!resId.empty()) {
      nlohmann::json resp;
      resp["message"] = "Reservation successful";
      resp["reservationId"] = resId;
      return http::HttpResponse::make(201, resp.dump());
    } else {
      return http::HttpResponse::make(400, 
        R"({"error": "Reservation failed. Tickets might be sold out."})");
    }

  } catch (...) {
    return http::HttpResponse::make(400, R"({"error": "Invalid request"})");
  }
}

http::HttpResponse EventController::handlePayment(const http::HttpRequest& req) {
  std::string userId;
  if (authMiddleware(req, security_, userId).statusCode != 200) {
    return http::HttpResponse::make(401, R"({"error": "Unauthorized"})");
  }

  try {
    auto json = nlohmann::json::parse(req.body);
    std::string resId = json["reservationId"];
    std::string nationalId = json.at("nationalId");
    double amount = json.at("amount");

    if (service_.processPayment(resId, nationalId, amount)) {
      return http::HttpResponse::make(200, 
        R"({"message": "Payment confirmed"})");
    } else {
      return http::HttpResponse::make(400, 
        R"({"error": "Payment failed or reservation expired"})");
    }

  } catch (...) {
    return http::HttpResponse::make(400, 
      R"({"error": "Invalid payment request"})");
  }
}

}  // namespace controller
