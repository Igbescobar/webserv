#include "server/HttpRequest.hpp"
#include "server/Server.hpp"
#include <iostream>
#include <string>

HttpRequest::HttpRequest() {
  std::cout << "constructor called" << std::endl;
}

void HttpRequest::append(std::string chunk) {
  buf += chunk;
}

bool HttpRequest::isCompleted() {
  size_t pos = buf.find(DELIMETER);
  if (pos == std::string::npos)
    return false;
  return true;
}
