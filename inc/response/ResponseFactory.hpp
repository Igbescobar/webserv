#pragma once

#include "parser_config/ServerConfig.hpp"
#include "request/HttpRequest.hpp"
#include "response/HttpResponse.hpp"

class ResponseFactory {
public:
  static HttpResponse make(const HttpRequest &req, const ServerConfig &config,
                           int statusCode);

  static HttpResponse makeWithoutRequest(const ServerConfig &config,
                                         int statusCode);

private:
  static std::string httpDate();
  static std::string connectionValue(const HttpRequest &req);
};