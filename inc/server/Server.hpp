#pragma once

#include "parser/config/ConfigParser.hpp"
#include "parser/config/ServerConfig.hpp"
#include "request/HttpRequest.hpp"
#include "response/HttpResponse.hpp"
#include "server/Epoll.hpp"
#include <map>
#include <string>
#include <sys/epoll.h>
#include <vector>

#define MAX_CONNECTIONS 10
#define BUF_SIZE 4096

class Server {
private:
  std::vector<int> serverFds;
  std::map<int, HttpRequest> requestMap;
  std::map<int, HttpResponse> responseMap;
  const ConfigParser &globalConfig;
  Epoll epoll;

  int socketCreate();
  void socketBind(int fd, std::string ip, int port);
  void socketListen(int fd);

  void handleEvents(int n);
  void handleSingleEvent(int fd, uint32_t eventsMask);

  void handleServer(int fd);
  void handleClient(int fd, uint32_t eventsMask);

  void clientRead(int fd);
  void clientWrite(int fd);

  void setNonBlocking(int fd);

  int numListeningSockets();
  unsigned int IPToNum(std::string ip);

  void startAllServers();
  void startSingleServer(std::string ip, int port);

  ServerConfig getServerConfig(int serverFd);

  void printServersFds();

public:
  Server(const ConfigParser &conf);
  ~Server();

  void run();
};
