#pragma once

#include "parser_config/LocationConfig.hpp"
#include "parser_config/ServerConfig.hpp"
#include "request/HttpRequest.hpp"
#include "response/HttpResponse.hpp"
#include <string>

class GetResponder {
public:
  static HttpResponse handle(const ServerConfig &config,
                             const LocationConfig *location,
                             const HttpRequest &req);

private:
  static HttpResponse handleDirectory(const ServerConfig &config,
                                      const LocationConfig *location,
                                      const std::string &uri,
                                      const std::string &dirPath);

  static std::string getIndexPath(const ServerConfig &config,
                                  const LocationConfig *location,
                                  const std::string &dirPath);

  static HttpResponse buildFileResponse(const std::string &filePath);
};