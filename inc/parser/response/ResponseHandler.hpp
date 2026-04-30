#ifndef RESPONSEHANDLER_HPP
#define RESPONSEHANDLER_HPP

#include "HttpResponse.hpp"
#include "parser/request/IRequest.hpp"
#include <string>

class IRequest;
class HttpResponse;

class ResponseHandler {
public:
  ResponseHandler(const std::string &docRoot);

  std::string handle(const IRequest &req);

private:
  std::string docRoot;

  std::string handleGet(const IRequest &req);
  std::string buildError(int statusCode, const std::string &message);

  static std::string joinPath(const std::string &base, const std::string &rel);
  static bool readFile(const std::string &path, std::string &out);
  static std::string guessContetType(const std::string &path);
};

#endif