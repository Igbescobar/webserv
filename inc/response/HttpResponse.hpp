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

#define STATUS_LINE "HTTP/1.1 200 OK\n"

class HttpResponse {
private:
  ServerConfig serverConfig;
  HttpRequest request;
  std::string response;
  int errorCode;

public:
  HttpResponse();
  HttpResponse(ServerConfig serverConfig, HttpRequest request);
  HttpResponse(ServerConfig serverConfig, int errorCode);
  HttpResponse(ServerConfig serverConfig, std::string cgiOutput);
  ~HttpResponse();
  HttpResponse(const HttpResponse &other);
  HttpResponse &operator=(const HttpResponse &other);

  std::string getResponse();
  void erase(int bytes);
  bool empty();
  bool isCgi();
  std::string getCgiPath();
  int getErrorCode();
};
