#pragma once

#include "parser/config/ServerConfig.hpp"
#include "request/HttpRequest.hpp"
#include "server/Epoll.hpp"

#define BUF_SIZE 4096
#define IDLE_LIMIT 30
#define ABSOLUTE_LIMIT 180

class Client {
private:
  int clientFd;
  Epoll &epoll;
  const ServerConfig &serverConfig;
  HttpRequest request;
  std::string responseStr;
  time_t connectionStart;
  time_t lastActivity;

  bool read(int clientFd);
  bool write(int clientFd);

  void updateActivity();

public:
  Client(int clientSocket, Epoll &epoll, const ServerConfig &serverConfig);
  ~Client();

  bool handleEvent(uint32_t eventsMask);
  bool isTimedOut();
};
