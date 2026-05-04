#ifndef IREQUEST_HPP
#define IREQUEST_HPP

#include <map>
#include <string>

class IRequest {
  public:
    virtual ~IRequest() {
    }

    virtual const std::string &getMethod() const = 0;
    virtual const std::string &getUri() const = 0;
    virtual const std::string &getVersion() const = 0;

    virtual std::string getHeader(const std::string &key) const = 0;
    virtual const std::map<std::string, std::string> &getHeaders() const = 0;

    virtual const std::string &getBody() const = 0;
};

#endif