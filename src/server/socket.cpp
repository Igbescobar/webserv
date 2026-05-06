#include "server/Server.hpp"
#include <cerrno>
#include <cstring>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>

int Server::socket_create() {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0)
    throw std::runtime_error("socket: " + std::string(strerror(errno)));

  int opt = 1;
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    throw std::runtime_error("setcokopt: " + std::string(strerror(errno)));

  return fd;
}

void Server::socket_bind(int fd, std::string ip, int port) {
  struct sockaddr_in addr;
  memset((char *)&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(IPToNum(ip));
  addr.sin_port = htons(port);
  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    throw std::runtime_error("bind: " + std::string(strerror(errno)));
}

void Server::socket_listen(int fd) {
  if (listen(fd, MAX_CONNECTIONS) < 0)
    throw std::runtime_error("listen: " + std::string(strerror(errno)));
}
