#pragma once

#include <string>

#define SOCKET_MAX_CONNECTIONS 10

using namespace std;

class Socket {
private:
  int fd;
  std::string ip;
  int port;

  int socketCreate();
  void socketBind(int fd, std::string ip, int port);
  void socketListen(int fd);
  unsigned int IPToNum(std::string ip);

public:
  Socket();
  Socket(std::string ip, int port);
  Socket(const Socket &other);
  Socket &operator=(const Socket &other);
  ~Socket();

  int getFd();
  static void setNonBlocking(int fd);
};
