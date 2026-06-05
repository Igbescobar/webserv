#pragma once

#include "parser/config/ServerConfig.hpp"
#include "request/HttpRequest.hpp"
#include "server/Epoll.hpp"

#define BUF_SIZE 4096
#define IDLE_LIMIT 30
#define ABSOLUTE_LIMIT 180
#define REQUEST_LIMIT 10000000

class Client {
private:
  int clientFd;
  Epoll &epoll;
  const ServerConfig &serverConfig;
  HttpRequest request;
  int requestSize;
  std::string responseStr;
  time_t connectionStart;
  time_t lastActivity;

  bool read(int clientFd);
  bool write(int clientFd);

  bool isCGI();
  const LocationConfig *getMatchingLocation(const std::string &uri);
  void updateActivity();

public:
  Client(int clientSocket, Epoll &epoll, const ServerConfig &serverConfig);
  ~Client();

  bool handleEvent(uint32_t eventsMask);
  bool isTimedOut();
};
