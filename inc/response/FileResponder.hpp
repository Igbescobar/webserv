#pragma once
#include "parser_config/ServerConfig.hpp"
#include <string>

class HttpRequest;

class FileResponder {
public:
  static HttpResponse handleGet(const ServerConfig &config,
                                const LocationConfig *location,
                                const HttpRequest &req);

private:
  static bool isSafeUri(const std::string &uri);
  static bool isDirectory(const std::string &path);
  static bool isRegularFile(const std::string &path);
  static std::string getDocumentRoot(const ServerConfig &config,
                                     const LocationConfig *location);

  static std::string getIndexPath(const ServerConfig &config,
                                  const LocationConfig *location,
                                  const std::string &dirPath);

  static HttpResponse handleDirectory(const ServerConfig &config,
                                      const LocationConfig *location,
                                      const std::string &uri,
                                      const std::string &dirPath);
  static HttpResponse buildFileResponse(const std::string &filePath);
};
