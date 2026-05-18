#pragma once

#include "parser/config/ConfigParser.hpp"
#include "parser/config/ServerConfig.hpp"
#include "server/Client.hpp"
#include "server/Epoll.hpp"
#include "server/Socket.hpp"
#include <csignal>
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
  static volatile sig_atomic_t isRunning;

  void handleEvents(int n);
  void handleSingleEvent(int fd, uint32_t eventsMask);

  void handleServer(int serverFd);

  void startAllServers();
  void startSingleServer(std::string ip, int port);

  const ServerConfig &getServerConfig(int serverFd) const;

  void sweepTimeouts();

public:
  Server(const ConfigParser &conf);
  ~Server();
  ConfigParser	getGlobalConfig(); 
  void run();
  static void sigintHandler(int signum);
};
