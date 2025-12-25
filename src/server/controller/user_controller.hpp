#pragma once

#include "controller.hpp"
#include "../service/user_service.hpp"
#include <nlohmann/json.hpp>

namespace controller {

// Controller class for User-related HTTP requests
class UserController : public RestController {
public:
  explicit UserController(service::UserService& service);

  // Main entry point for handling an HTTP request
  http::HttpResponse handleRequest(const http::HttpRequest& req) override;

private:
  service::UserService& service_;

  http::HttpResponse handleGet(const http::HttpRequest& req);
  http::HttpResponse handlePost(const http::HttpRequest& req);
  http::HttpResponse handlePut(const http::HttpRequest& req);
  http::HttpResponse handleDelete(const http::HttpRequest& req);
};

}  // namespace controller