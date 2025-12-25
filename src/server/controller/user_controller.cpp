#include "user_controller.hpp"
#include <iostream>

namespace controller {

UserController::UserController(service::UserService& service) 
  : service_(service) {}

http::HttpResponse UserController::handleRequest(const http::HttpRequest& req) {
  try {
    switch (req.method) {
      case http::Method::GET:    
        return handleGet(req);
      case http::Method::POST:
        return handlePost(req);
      case http::Method::PUT:   
        return handlePut(req);
      case http::Method::DELETE: 
        return handleDelete(req);
      default: 
        return http::HttpResponse::make(405, "Method Not Allowed");
    }
  } catch (const std::exception& e) {
    nlohmann::json error = {{"error", e.what()}};
    return http::HttpResponse::make(500, error.dump());
  }
}

http::HttpResponse UserController::handleGet(const http::HttpRequest& req) {
  if (req.queryParams.count("id")) {
    std::string id = req.queryParams.at("id");
    auto user = service_.getById(id);
    if (user) {
      nlohmann::json jsonUser = *user;
      return http::HttpResponse::make(200, jsonUser.dump());
    } else {
      return http::HttpResponse::make(404, R"({"error": "User not found"})");
    }
  }

  // No ID, return all users
  auto users = service_.getAll();
  nlohmann::json jsonUsers = users;
  return http::HttpResponse::make(200, jsonUsers.dump());
}

http::HttpResponse UserController::handlePost(const http::HttpRequest& req) {
  try {
    auto jsonBody = nlohmann::json::parse(req.body);
    models::User user = jsonBody.get<models::User>();
    auto createdUser = service_.create(user);
    nlohmann::json response = createdUser;
    return http::HttpResponse::make(201, response.dump());

  } catch (const nlohmann::json::exception&) {
    return http::HttpResponse::make(400, R"({"error": "Invalid JSON format"})");
  }
}

http::HttpResponse UserController::handlePut(const http::HttpRequest& req) {
  try {
    auto jsonBody = nlohmann::json::parse(req.body);
    models::User user = jsonBody.get<models::User>();
    auto updatedUser = service_.update(user);
    nlohmann::json response = updatedUser;
    return http::HttpResponse::make(200, response.dump());
      
  } catch (const nlohmann::json::exception&) {
    return http::HttpResponse::make(400, R"({"error": "Invalid JSON format"})");
  }
}

http::HttpResponse UserController::handleDelete(const http::HttpRequest& req) {
  if (req.queryParams.count("id")) {
    std::string id = req.queryParams.at("id");
    service_.deleteById(id);
    return http::HttpResponse::make(204);
  }
  return http::HttpResponse::make(400, 
    R"({"error": "Missing 'id' parameter"})");
}

}  // namespace controller
