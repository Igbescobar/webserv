#ifndef RESPONSEHANDLER_HPP
#define RESPONSEHANDLER_HPP

#include "parser_config/ServerConfig.hpp"
#include "request/IRequest.hpp"
#include <string>

class ResponseHandler {
public:
  ResponseHandler(const ServerConfig &config, const IRequest &req);
  ResponseHandler(const ServerConfig &config, int errorCode);

  std::string handle();

private:
  const ServerConfig &config;
  const IRequest *req;
  const int errorCode;

  std::string handleGet();

  bool isMethodAllowed(const LocationConfig *location,
                       const std::string &method) const;
  std::string handleRedirect(const LocationConfig *location) const;
};

#endif