#pragma once

#include "parser/config/ServerConfig.hpp"
#include "request/HttpRequest.hpp"
#include "server/Epoll.hpp"

#define BUF_SIZE 4096

class Client {
private:
  int clientFd;
  Epoll &epoll;
  const ServerConfig &serverConfig;
  HttpRequest request;
  std::string responseStr;

  bool read(int clientFd);
  bool write(int clientFd);

public:
  Client(int clientSocket, Epoll &epoll, const ServerConfig &serverConfig);
  ~Client();

  bool handleEvent(uint32_t eventsMask);
};
