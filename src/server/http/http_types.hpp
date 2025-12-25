#pragma once

#include <string>
#include <unordered_map>

namespace http {

// Supported HTTP Methods
enum class Method {
  GET,
  POST,
  PUT,
  DELETE,
  OPTIONS,
  UNKNOWN
};

struct HttpRequest {
  Method method;
  std::string path;
  std::unordered_map<std::string, std::string> headers;
  std::string body;
  std::unordered_map<std::string, std::string> queryParams;

  // Helper to get method from string
  static Method stringToMethod(const std::string& m) {
    if (m == "GET") return Method::GET;
    if (m == "POST") return Method::POST;
    if (m == "PUT") return Method::PUT;
    if (m == "DELETE") return Method::DELETE;
    if (m == "OPTIONS") return Method::OPTIONS;
    return Method::UNKNOWN;
  }
};

struct HttpResponse {
  int statusCode;
  std::string body;
  std::unordered_map<std::string, std::string> headers = {
    {"Content-Type", "application/json"}
  };

  static HttpResponse make(int code, const std::string& body = "") {
    return {code, body};
  }
};

} // namespace http