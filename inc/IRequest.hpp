#ifndef IREQUEST_HPP
#define IREQUEST_HPP

#include <map>
#include <string>

class IRequest {
public:
  virtual ~IRequest() {}

  virtual std::string getMethod() const = 0;
  virtual std::string getUri() const = 0;
  virtual std::string getVersion() const = 0;

  //virtual std::string getHeader(const std::string &key) const = 0;
  //virtual std::map<std::string, std::string> getHeaders() const = 0;

  virtual std::string getBody() const = 0;
};

#endif
