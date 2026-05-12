#include "request/HttpRequest.hpp"
#include "parser_config/ServerConfig.hpp"
#include <string>
#include <sstream>

HttpRequest::HttpRequest() {}

HttpRequest::HttpRequest(ServerConfig serverConfig)
    : serverConfig(serverConfig), method("GET"), uri("/") {
  errorCode = -1;
  state = INCOMPLETE;
}

HttpRequest::~HttpRequest() {}

HttpRequest::HttpRequest(const HttpRequest &other) {
  serverConfig = other.serverConfig;
  buf = other.buf;
  state = other.state;
  errorCode = other.errorCode;
  method = other.method;
  uri = other.uri;
}

HttpRequest &HttpRequest::operator=(const HttpRequest &other) {
  serverConfig = other.serverConfig;
  buf = other.buf;
  state = other.state;
  errorCode = other.errorCode;
  method = other.method;
  uri = other.uri;
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
  
  if (state == INCOMPLETE) {
      std::istringstream stream(buf);
      stream >> method >> uri;
  }
  
  state = COMPLETE;
}

const ServerConfig& HttpRequest::getServerConfig() const { return serverConfig; }

t_state HttpRequest::getState() { return state; }

int HttpRequest::getErrorCode() { return errorCode; }

std::string HttpRequest::getRequest() { return buf; }

std::string HttpRequest::getUri() const { return uri; }

std::string HttpRequest::getMethod() const { return method; }
