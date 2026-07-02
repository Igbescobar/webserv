#pragma once

#include "parser_config/LocationConfig.hpp"
#include "parser_config/ServerConfig.hpp"
#include <string>

class HttpRequest;

struct CgiTarget {
  bool isCgi;
  std::string scriptPath;
  const LocationConfig *location;

  CgiTarget() : isCgi(false), scriptPath(""), location(NULL) {}
};

class CgiTargetResolver {
public:
  static CgiTarget resolve(const ServerConfig &config, const HttpRequest &req);

private:
  static std::string stripQuery(const std::string &uri);
  static std::string extractExtension(const std::string &uriPath);
  static bool hasAllowedExtension(const LocationConfig &location,
                                  const std::string &extension);
  static bool isMethodAllowed(const LocationConfig *location,
                              const std::string &method);
  static bool isUriUnderLocation(const std::string &uriPath,
                                 const std::string &pattern);
  static std::string trimLocationPrefix(const std::string &uriPath,
                                        const std::string &pattern);
  static std::string buildScriptPath(const ServerConfig &config,
                                     const LocationConfig &location,
                                     const std::string &uriPath);
};
