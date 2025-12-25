#pragma once

#include "http/http_types.hpp"

namespace controller {

// Base interface for RESTful controllers
class RestController {
public:
    virtual ~RestController() = default;

    // Main entry point for handling an HTTP request
    virtual http::HttpResponse handleRequest(const http::HttpRequest& req) = 0;
};

}  // namespace controller