#pragma once
#include "parser_config/ServerConfig.hpp"
#include "request/IRequest.hpp"
#include <string>

class FileResponder {
public:
  static std::string handleGet(const ServerConfig &config,
                               const LocationConfig *location,
                               const IRequest &req);

private:
  static bool isSafeUri(const std::string &uri);
  static bool isDirectory(const std::string &path);
  static bool isRegularFile(const std::string &path);
  static std::string getDocumentRoot(const ServerConfig &config,
                                     const LocationConfig *location);

  static std::string getIndexPath(const ServerConfig &config,
                                  const LocationConfig *location,
                                  const std::string &dirPath);

  static std::string handleDirectory(const ServerConfig &config,
                                     const LocationConfig *location,
                                     const std::string &uri,
                                     const std::string &dirPath);
  static std::string buildFileResponse(const std::string &filePath);
};
