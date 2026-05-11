#ifndef MOCKREQUEST_HPP
#define MOCKREQUEST_HPP

#include "request/HttpRequest.hpp"
#include <map>
#include <string>

class MockRequest : public HttpRequest {
private:
  std::string method_;
  std::string uri_;
  std::string version_;
  std::string body_;
  std::map<std::string, std::string> headers_;

public:
  MockRequest(const std::string &m, const std::string &u)
      : HttpRequest(), method_(m), uri_(u), version_("HTTP/1.1"), body_("") {}

  // These are the only ones ResponseHandler needs for now
  std::string getMethod() const { return method_; }
  std::string getUri() const { return uri_; }

  // Optional helpers if your code/tests use them
  const std::string &getVersion() const { return version_; }

  std::string getHeader(const std::string &key) const {
    std::map<std::string, std::string>::const_iterator it = headers_.find(key);
    if (it != headers_.end())
      return it->second;
    return "";
  }

  const std::map<std::string, std::string> &getHeaders() const {
    return headers_;
  }

  const std::string &getBody() const { return body_; }
};

#endif