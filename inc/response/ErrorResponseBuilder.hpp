#pragma once

#include "parser_config/ServerConfig.hpp"
#include <string>
#include "HttpResponse.hpp"

class ErrorResponseBuilder {
public:
  static HttpResponse build(const ServerConfig &config, int statusCode);
  static HttpResponse build(const ServerConfig &config, int statusCode,
                            const std::string &message);

private:
  static const char *reasonPhrase(int statusCode);

  static bool tryBuildConfiguredErrorPage(const ServerConfig &config,
                                          int statusCode, std::string &outBody,
                                          std::string &outContentType);

  static std::string defaultErrorHtml(int statusCode,
                                      const std::string &message);
};
