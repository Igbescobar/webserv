#ifndef ERRORRESPONSEBUILDER_HPP
#define ERRORRESPONSEBUILDER_HPP

#include "parser_config/ServerConfig.hpp"
#include <string>

class ErrorResponseBuilder {
public:
  static std::string build(const ServerConfig &config, int statusCode);
  static std::string build(const ServerConfig &config, int statusCode,
                           const std::string &message);

private:
  static const char *reasonPhrase(int statusCode);

  static bool tryBuildConfiguredErrorPage(const ServerConfig &config,
                                          int statusCode, std::string &outBody,
                                          std::string &outContentType);

  static std::string defaultErrorHtml(int statusCode,
                                      const std::string &message);
};

#endif