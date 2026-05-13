#pragma once

#include "parser/config/ConfigParser.hpp"
#include "parser/config/ServerConfig.hpp"
#include "server/Client.hpp"
#include "server/Epoll.hpp"
#include "server/Socket.hpp"
#include <map>
#include <string>
#include <sys/epoll.h>
#include <vector>

class Server {
private:
  std::vector<Socket *> Sockets;
  std::map<int, Client *> clientMap;
  const ConfigParser &globalConfig;
  Epoll epoll;

  void handleEvents(int n);
  void handleSingleEvent(int fd, uint32_t eventsMask);

  void handleServer(int serverFd);
  void handleClient(int fd, uint32_t eventsMask);

  void clientRead(int fd);
  void clientWrite(int fd);

  void startAllServers();
  void startSingleServer(std::string ip, int port);

  ServerConfig getServerConfig(int serverFd); // TODO: use reference

public:
  Server(const ConfigParser &conf);
  ~Server();

  void run();
};
