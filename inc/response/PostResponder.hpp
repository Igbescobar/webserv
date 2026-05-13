#pragma once

#include "parser_config/LocationConfig.hpp"
#include "parser_config/ServerConfig.hpp"
#include "request/HttpRequest.hpp"
#include "response/HttpResponse.hpp"

class PostResponder {
public:
  static HttpResponse handle(const ServerConfig &config,
                             const LocationConfig *location,
                             const HttpRequest &req);

private:
  static bool ensureDirExists(const std::string &dirPath);
  static std::string baseNameFromUri(const std::string &uri);
};