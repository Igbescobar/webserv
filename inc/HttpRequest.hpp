#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "IRequest.hpp"
#include <cctype>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

class ParseError : public std::runtime_error {
public:
  ParseError(const std::string &msg) : std::runtime_error(msg) {}
};

class HttpRequest : public IRequest {
private:
  std::string method;
  std::string uri;
  std::string version;
  std::map<std::string, std::string> headers;
  std::string body;

  void parseRawData(const std::string &rawData);

public:
  HttpRequest(const std::string &rawData);
  ~HttpRequest();

  std::string getMethod() const;
  std::string getUri() const;
  std::string getVersion() const;
  std::string getHeader(const std::string &key) const;
  std::map<std::string, std::string> getHeaders() const;
  std::string getBody() const;
};

#endif
