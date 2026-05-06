#pragma once

#include "parser/config/ServerConfig.hpp"

class HttpResponse {
private:
  ServerConfig serverConfig;
  std::string request;

public:
  HttpResponse(ServerConfig &serverConfig, const std::string &request);
  // HttpResponse(ServerConfig &serverConfig, int errorCode);
};
