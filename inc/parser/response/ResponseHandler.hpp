#ifndef RESPONSEHANDLER_HPP
#define RESPONSEHANDLER_HPP

#include "parser/config/ServerConfig.hpp"
#include "parser/request/IRequest.hpp"
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
};

#endif