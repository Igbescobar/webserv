#ifndef FILERESPONDER_HPP
#define FILERESPONDER_HPP

#include "parser/config/ServerConfig.hpp"
#include "parser/request/IRequest.hpp"
#include <string>

class FileResponder {
public:
  static std::string handleGet(const ServerConfig &config, const IRequest &req);

private:
  static bool isSafeUri(const std::string &uri);
  static bool isDirectory(const std::string &path);
  static bool isRegularFile(const std::string &path);
  static std::string getIndexPath(const ServerConfig &config,
                                  const std::string &dirPath);

  static std::string buildFileResponse(const std::string &filePath);
};

#endif