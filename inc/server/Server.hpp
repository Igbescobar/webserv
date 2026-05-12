#pragma once

#include "parser/config/ConfigParser.hpp"
#include "parser/config/ServerConfig.hpp"
#include "request/HttpRequest.hpp"
#include "response/HttpResponse.hpp"
#include "server/Epoll.hpp"
#include "server/Socket.hpp"
#include <map>
#include <string>
#include <sys/epoll.h>
#include <vector>

#define BUF_SIZE 4096

using namespace std;

class Server {
private:
  vector<Socket *> Sockets;
  map<int, HttpRequest> requestMap;
  map<int, HttpResponse> responseMap;
  const ConfigParser &globalConfig;
  Epoll epoll;

  void handleEvents(int n);
  void handleSingleEvent(int fd, uint32_t eventsMask);

  void handleServer(int fd);
  void handleClient(int fd, uint32_t eventsMask);

  void clientRead(int fd);
  void clientWrite(int fd);

  void startAllServers();
  void startSingleServer(string ip, int port);

  ServerConfig getServerConfig(int serverFd);

public:
  Server(const ConfigParser &conf);
  ~Server();

  void run();
};
