#include "request/HttpRequest.hpp"
#include "parser_config/ServerConfig.hpp"
#include <string>

HttpRequest::HttpRequest() {}

HttpRequest::HttpRequest(ServerConfig serverConfig)
    : serverConfig(serverConfig) {
  errorCode = -1;
  state = INCOMPLETE;
}

HttpRequest::~HttpRequest() {}

HttpRequest::HttpRequest(const HttpRequest &other) {
  serverConfig = other.serverConfig;
  buf = other.buf;
  state = other.state;
  errorCode = other.errorCode;
}

HttpRequest &HttpRequest::operator=(const HttpRequest &other) {
  serverConfig = other.serverConfig;
  buf = other.buf;
  state = other.state;
  errorCode = other.errorCode;
  return *this;
}

void HttpRequest::append(std::string chunk) {
  buf += chunk;
  updateState();
}

void HttpRequest::updateState() {
  size_t pos = buf.find(DELIMETER);
  if (pos == std::string::npos) {
    state = INCOMPLETE;
    return;
  }
  state = COMPLETE;
}

ServerConfig HttpRequest::getServerConfig() { return serverConfig; }

t_state HttpRequest::getState() { return state; }

int HttpRequest::getErrorCode() { return errorCode; }

std::string HttpRequest::getRequest() { return buf; }

std::string HttpRequest::getUri() const { return "/"; }

std::string HttpRequest::getMethod() const { return "GET"; }