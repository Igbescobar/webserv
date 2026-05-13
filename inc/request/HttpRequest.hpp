#pragma once

#include "parser_config/ServerConfig.hpp"
#include <string>

#define DELIMETER "\r\n\r\n"

typedef enum e_state { INCOMPLETE, COMPLETE, ERROR } t_state;

class HttpRequest {
private:
  ServerConfig serverConfig;
  std::string buf;
  t_state state;
  int errorCode;
  std::string method;
  std::string uri;
  std::string body;

  void updateState();

public:
  HttpRequest();
  HttpRequest(ServerConfig serverConfig);
  ~HttpRequest();
  HttpRequest(const HttpRequest &other);
  HttpRequest &operator=(const HttpRequest &other);

  void append(std::string chunk);

  t_state getState();

  int getErrorCode();

  const ServerConfig &getServerConfig() const;

  std::string getRequest();

  std::string getUri() const;
  std::string getMethod() const;
  std::string getBody() const;
};
