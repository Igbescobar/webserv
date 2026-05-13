#pragma once

#include "parser_config/LocationConfig.hpp"
#include "parser_config/ServerConfig.hpp"
#include "request/HttpRequest.hpp"
#include "response/HttpResponse.hpp"

class DeleteResponder {
public:
  static HttpResponse handle(const ServerConfig &config,
                             const LocationConfig *location,
                             const HttpRequest &req);
};