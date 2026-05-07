#pragma once

#include "parser/config/ServerConfig.hpp"
#include "request/HttpRequest.hpp"
#include <string>

#define RESPONSE                                                               \
  "HTTP/1.1 200 OK\n"                                                          \
  "Content-Type: text/plain\n"                                                 \
  "Content-Length: 13\n"                                                       \
  "\n"                                                                         \
  "Hello world!\n"

class HttpResponse {
private:
  ServerConfig serverConfig;
  HttpRequest request;
  std::string response;

public:
  HttpResponse(ServerConfig serverConfig, HttpRequest request);
  // HttpResponse(ServerConfig &serverConfig, int errorCode);
  std::string getResponse();
  void erase(int bytes);
};
