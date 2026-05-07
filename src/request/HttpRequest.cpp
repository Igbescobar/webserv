#include "request/HttpRequest.hpp"
#include "parser/config/ServerConfig.hpp"
#include "server/Server.hpp"
#include <string>

HttpRequest::HttpRequest() {}

HttpRequest::HttpRequest(ServerConfig serverConfig)
    : serverConfig(serverConfig) {}

HttpRequest::~HttpRequest() {}

HttpRequest::HttpRequest(const HttpRequest &other) {
  serverConfig = other.serverConfig;
  buf = other.buf;
}

HttpRequest &HttpRequest::operator=(const HttpRequest &other) {
  serverConfig = other.serverConfig;
  buf = other.buf;
  return *this;
}

void HttpRequest::append(std::string chunk) { buf += chunk; }

bool HttpRequest::isCompleted() {
  size_t pos = buf.find(DELIMETER);
  if (pos == std::string::npos)
    return false;
  return true;
}
