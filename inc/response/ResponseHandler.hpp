#pragma once

#include "parser_config/ServerConfig.hpp"
#include <string>

class HttpRequest;
class HttpResponse;

class ResponseHandler {
public:
  ResponseHandler(const ServerConfig &config, const HttpRequest &req);
  ResponseHandler(const ServerConfig &config, int errorCode);

  HttpResponse handle();

private:
  const ServerConfig &config;
  const HttpRequest *req;
  const int errorCode;

  HttpResponse handleGet();

  bool isMethodAllowed(const LocationConfig *location,
                       const std::string &method) const;
  bool handleRedirect(const LocationConfig *location,
                      HttpResponse &response) const;
};