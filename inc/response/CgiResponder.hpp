#pragma once

#include "parser_config/ServerConfig.hpp"
#include "request/HttpRequest.hpp"
#include "response/HttpResponse.hpp"
#include <string>

class CgiResponder {
public:
  static HttpResponse handle(const ServerConfig &config, const HttpRequest &req,
                             const std::string &cgiOutput);

private:
  static void applyCgiHeaders(HttpResponse &resp,
                              const std::string &cgiHeaders);
};
