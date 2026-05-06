#include "server/HttpRequest.hpp"
#include "parser/config/ServerConfig.hpp"
#include "server/Server.hpp"
#include <iostream>
#include <string>

HttpRequest::HttpRequest(ServerConfig &serverConfig)
    : serverConfig(serverConfig) {
  std::cout << "HttpRequest() called" << std::endl;
}

void HttpRequest::append(std::string chunk) { buf += chunk; }

bool HttpRequest::isCompleted() {
  size_t pos = buf.find(DELIMETER);
  if (pos == std::string::npos)
    return false;
  return true;
}
