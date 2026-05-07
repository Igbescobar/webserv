#pragma once

#include "parser/config/ConfigParser.hpp"
#include "parser/config/ServerConfig.hpp"
#include "request/HttpRequest.hpp"
#include <map>
#include <string>
#include <sys/epoll.h>
#include <vector>

#define MAX_CONNECTIONS 10
#define MAX_EVENTS 10
#define BUF_SIZE 4096

// TODO:
// cleanup resources: fd, etc (do not leak fd)
// ctrl-c safe exit
// error handling
// control never-ending request
// control timeout request
// control request too large
// do not stop if max open files reached

class Server {
private:
  std::vector<int> servers;
  int epoll_fd;
  struct epoll_event events[MAX_EVENTS];
  std::map<int, HttpRequest> requestMap;
  std::map<int, std::string> responseMap;
  ConfigParser globalConfig;

  int socket_create();
  void socket_bind(int fd, std::string ip, int port);
  void socket_listen(int fd);

  void handle_event(int fd, uint32_t events);
  void handle_events(int n);

  void handle_server(int fd);
  void handle_client(int fd, uint32_t events);

  void client_read(int fd);
  void client_write(int fd);

  void epoll_create();
  void epoll_read(int fd);
  void epoll_write(int fd);
  void epoll_remove(int fd);

  void non_blocking(int fd);

  int numListeningSockets();
  unsigned int IPToNum(std::string ip);

  void startAllServers();
  void startServer(std::string ip, int port);

  ServerConfig getServerConfig(int server_fd);

  void printServersFds();

public:
  Server(const ConfigParser &conf);
  ~Server();

  void run();
};
