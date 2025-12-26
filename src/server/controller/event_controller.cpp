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
  if (req.path.find("/events") != std::string::npos && 
      req.method == http::Method::GET) {
    return handleGet(req);
  } else {
    return http::HttpResponse::make(405, "Method Not Allowed");
  }
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

}  // namespace controller
