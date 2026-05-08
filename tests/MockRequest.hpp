#ifndef MOCKREQUEST_HPP
#define MOCKREQUEST_HPP

#include "request/IRequest.hpp"
#include <map>

class MockRequest : public IRequest {
private:
  std::string method;
  std::string uri;
  std::string version;
  std::string body;
  std::map<std::string, std::string> headers;

public:
  MockRequest(std::string m, std::string u)
      : method(m), uri(u), version("HTTP/1.1") {}

  const std::string &getMethod() const { return method; }
  const std::string &getUri() const { return uri; }
  const std::string &getVersion() const { return version; }

  std::string getHeader(const std::string &key) const {
    std::map<std::string, std::string>::const_iterator it = headers.find(key);
    if (it != headers.end())
      return it->second;
    return "";
  }
  const std::map<std::string, std::string> &getHeaders() const {
    return headers;
  }
  const std::string &getBody() const { return body; }
};

#endif