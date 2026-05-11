#pragma once

#include "parser/config/ConfigParser.hpp"
#include "parser/config/ServerConfig.hpp"
#include "request/HttpRequest.hpp"
#include "response/HttpResponse.hpp"
#include <map>
#include <string>
#include <sys/epoll.h>
#include <vector>

#define MAX_CONNECTIONS 10
#define MAX_EVENTS 10
#define BUF_SIZE 4096

class Server {
private:
  std::vector<int> serverFds;
  int epollFd;
  struct epoll_event epollEvents[MAX_EVENTS];
  std::map<int, HttpRequest> requestMap;
  std::map<int, HttpResponse> responseMap;
  const ConfigParser &globalConfig;

  int socketCreate();
  void socketBind(int fd, std::string ip, int port);
  void socketListen(int fd);

  void handleEvents(int n);
  void handleEvent(int fd, uint32_t eventsMask);

  void handleServer(int fd);
  void handleClient(int fd, uint32_t eventsMask);

  void clientRead(int fd);
  void clientWrite(int fd);

  void epollCreate();
  void epollAddRead(int fd);
  void epollModWrite(int fd);
  void epollRemove(int fd);
  int epollWait();

  void setNonBlocking(int fd);

  int numListeningSockets();
  unsigned int IPToNum(std::string ip);

  void startAllServers();
  void startServer(std::string ip, int port);

  ServerConfig getServerConfig(int serverFd);

  void printServersFds();

public:
  Server(const ConfigParser &conf);
  ~Server();

  void run();
};
