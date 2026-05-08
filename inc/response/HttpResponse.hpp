#pragma once

#include "IResponse.hpp"
#include <map>
#include <string>

class ServerConfig;
class IRequest;

class HttpResponse : public IResponse {
private:
  std::string version;
  int statusCode;
  std::string statusMessage;
  std::map<std::string, std::string> headers;
  std::string body;

  std::string rawResponse;
  bool isBuilt;

  void setStatusMessage(int code);

public:
  HttpResponse();
  HttpResponse(const HttpResponse &other);
  HttpResponse &operator=(const HttpResponse &other);
  ~HttpResponse();

  HttpResponse(const ServerConfig &config, const IRequest &req);
  HttpResponse(const ServerConfig &config, int errorCode);

  void setStatusCode(int code);
  void setHeader(const std::string &key, const std::string &value);
  void setBody(const std::string &body);

  std::string toString() const;

  std::string getResponse();
  void erase(int bytes);
  bool empty() const;
};