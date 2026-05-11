#include "server/Server.hpp"
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>

int Server::socketCreate() {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0)
    throw std::runtime_error("socket: " + std::string(strerror(errno)));

  int opt = 1;
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    throw std::runtime_error("setsokopt: " + std::string(strerror(errno)));

  return fd;
}

void Server::socketBind(int fd, std::string ip, int port) {
  struct sockaddr_in addr;
  memset((char *)&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(IPToNum(ip));
  addr.sin_port = htons(port);
  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    throw std::runtime_error("bind: " + std::string(strerror(errno)));
}

void Server::socketListen(int fd) {
  if (listen(fd, MAX_CONNECTIONS) < 0)
    throw std::runtime_error("listen: " + std::string(strerror(errno)));
}

void Server::setNonBlocking(int fd) {
  if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
    throw std::runtime_error("fcntl: " + std::string(strerror(errno)));
}
