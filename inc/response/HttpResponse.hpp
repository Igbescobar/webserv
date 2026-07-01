#pragma once

#include <map>
#include <string>

class ServerConfig;

class HttpResponse {
private:
  std::string version;
  int statusCode;
  std::string statusMessage;
  std::map<std::string, std::string> headers;
  std::string body;

  std::string rawResponse;
  bool isBuilt;

  void setStatusMessage(int code);
  std::string toString() const;

public:
  HttpResponse();
  HttpResponse(const HttpResponse &other);
  HttpResponse &operator=(const HttpResponse &other);
  ~HttpResponse();

  void setStatusCode(int code);
  void setHeader(const std::string &key, const std::string &value);
  void setBody(const std::string &body);

  void clearBody();

  std::string getResponse();
};