#ifndef RESPONSEHANDLER_HPP
#define RESPONSEHANDLER_HPP

#include "HttpResponse.hpp"
#include "parser/config/ServerConfig.hpp"
#include "parser/request/IRequest.hpp"
#include <string>

class IRequest;
class HttpResponse;

class ResponseHandler {
public:
  ResponseHandler(const ServerConfig &config);

  std::string handle(const IRequest &req);

private:
  ServerConfig config;
  std::string handleGet(const IRequest &req);
  std::string buildError(int statusCode, const std::string &message);

  bool isSafeUri(const std::string &uri) const;
  bool isDirectory(const std::string &path) const;
  bool isRegularFile(const std::string &path) const;
  std::string getIndexPath(const std::string &dirPath) const;
  std::string buildFileResponse(const std::string &filePath);

  static std::string joinPath(const std::string &base, const std::string &rel);
  static bool readFile(const std::string &path, std::string &out);
  static std::string guessContentType(const std::string &path);
};

#endif