#pragma once

#include "parser/config/ServerConfig.hpp"
#include "server/Epoll.hpp"

#define CLIENT_DELETE 1
#define BUF_SIZE 4096

class Client {
private:
  int clientFd;
  Epoll &epoll;
  ServerConfig &serverConfig;

  int read(int clientFd);
  int write(int clientFd);

public:
  Client(int clientSocket, Epoll &epoll, ServerConfig &serverConfig);
  ~Client();

  int handle(uint32_t eventsMask);
};
