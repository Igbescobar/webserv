#pragma once

#include <string>

class IResponse {
public:
  virtual ~IResponse() {}

  virtual void setStatusCode(int code) = 0;
  virtual void setHeader(const std::string &key, const std::string &value) = 0;
  virtual void setBody(const std::string &body) = 0;

  virtual std::string toString() const = 0;
};