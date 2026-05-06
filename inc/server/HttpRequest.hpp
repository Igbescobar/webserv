#pragma once

#include "parser/config/ServerConfig.hpp"
#include <string>

class HttpRequest {
private:
  ServerConfig &serverConfig;
  std::string buf;

public:
  HttpRequest(ServerConfig &serverConfig);

  void append(std::string chunk);

  bool isCompleted();
};
