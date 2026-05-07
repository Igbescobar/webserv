#pragma once

#include "parser/config/ServerConfig.hpp"
#include <string>

#define DELIMETER "\r\n\r\n"

class HttpRequest {
private:
  ServerConfig serverConfig;
  std::string buf;

public:
  HttpRequest();
  HttpRequest(ServerConfig serverConfig);
  ~HttpRequest();
  HttpRequest(const HttpRequest &other);
  HttpRequest &operator=(const HttpRequest &other);

  void append(std::string chunk);

  bool isCompleted();
};
