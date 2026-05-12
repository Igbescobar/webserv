#include "server/Socket.hpp"
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

Socket::Socket() {}

Socket::Socket(string ip, int port) {
  fd = socketCreate();
  setNonBlocking(fd);
  socketBind(fd, ip, port);
  socketListen(fd);
}

Socket::~Socket() { close(fd); }

Socket::Socket(const Socket &other) {
  fd = other.fd;
  ip = other.ip;
  port = other.port;
}

Socket &Socket::operator=(const Socket &other) {
  fd = other.fd;
  ip = other.ip;
  port = other.port;
  return *this;
}

int Socket::socketCreate() {
  int socketFd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketFd < 0)
    throw runtime_error("socket: " + string(strerror(errno)));

  int opt = 1;
  if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    throw runtime_error("setsokopt: " + string(strerror(errno)));

  return socketFd;
}

void Socket::socketBind(int fd, string ip, int port) {
  struct sockaddr_in addr;
  memset((char *)&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(IPToNum(ip));
  addr.sin_port = htons(port);
  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    throw runtime_error("bind: " + string(strerror(errno)));
}

void Socket::socketListen(int fd) {
  if (listen(fd, SOCKET_MAX_CONNECTIONS) < 0)
    throw runtime_error("listen: " + string(strerror(errno)));
}

unsigned int Socket::IPToNum(string ip) {
  stringstream ss(ip);
  unsigned int a, b, c, d;
  char dot;

  ss >> a >> dot >> b >> dot >> c >> dot >> d;

  return a << 24 | b << 16 | c << 8 | d;
}

int Socket::getFd() { return fd; }

void Socket::setNonBlocking(int fd) {
  if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
    throw runtime_error("fcntl: " + string(strerror(errno)));
}
