#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "IResponse.hpp"
#include <map>
#include <string>

class HttpResponse : public IResponse {
private:
  std::string version;
  int statusCode;
  std::string statusMessage;
  std::map<std::string, std::string> headers;
  std::string body;

  void setStatusMessage(int code);

public:
  HttpResponse();
  ~HttpResponse();

  void setStatusCode(int code);
  void setHeader(const std::string &key, const std::string &value);
  void setBody(const std::string &body);

  std::string toString() const;
};

#endif
