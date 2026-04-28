#include "server/Server.hpp"
#include "parser/config/ConfigParser.hpp"
#include "parser/config/ServerConfig.hpp"
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>

Server::Server(const ConfigParser &config) : conf(config) {
  const ServerConfig *sc;
  epoll_create();

  for (size_t i = 0; i < conf.getServerConfigs().size(); i++) {
    sc = &conf.getServerConfigs()[i];
    for (size_t j = 0; j < sc->getIPs().size(); j++) {
      servers.push_back(socket_create());
      non_blocking(servers[servers.size() - 1]);
      socket_bind(servers[servers.size() - 1], sc->getIPs()[j],
                  sc->getPorts()[j]);
      socket_listen(servers[servers.size() - 1]);
      epoll_read(servers[servers.size() - 1]);
    }
  }
}

Server::~Server() {}

void Server::non_blocking(int fd) {
  if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
    throw std::runtime_error("fcntl: " + std::string(strerror(errno)));
}

void Server::run() {
  while (true) {
    int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    if (num_events < 0)
      throw std::runtime_error("epoll_wait: " + std::string(strerror(errno)));
    handle_events(num_events);
  }
}

void Server::handle_events(int n) {
  for (int i = 0; i < n; i++)
    handle_event(events[i].data.fd, events[i].events);
}

void Server::handle_event(int fd, uint32_t events) {
  for (size_t i = 0; i < servers.size(); i++) {
    if (fd == servers[i]) {
      handle_server(servers[i]);
      return;
    }
  }
  handle_client(fd, events);
}

void Server::handle_server(int fd) {
  int new_socket;
  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);

  new_socket = accept(fd, (struct sockaddr *)&addr, &addr_len);
  if (new_socket < 0)
    throw std::runtime_error("accept: " + std::string(strerror(errno)));

  non_blocking(new_socket);
  read_map.erase(new_socket);
  epoll_read(new_socket);
  return;
}

void Server::handle_client(int fd, uint32_t events) {
  if (events & EPOLLIN)
    client_read(fd);
  else if (events & EPOLLOUT)
    client_write(fd);
}

void Server::client_read(int fd) {
  char buffer[BUF_SIZE + 1];
  int bytes_read;

  bytes_read = read(fd, buffer, BUF_SIZE);
  if (bytes_read <= 0) {
    epoll_remove(fd);
    read_map.erase(fd);
    close(fd);
    return;
  }

  buffer[bytes_read] = '\0';
  read_map[fd] += buffer;

  size_t pos = read_map[fd].find(DELIMETER);
  if (pos == std::string::npos)
    return;

  read_map.erase(fd);

  epoll_write(fd);
}

void Server::client_write(int fd) {
  int bytes_written;

  if (write_map[fd].empty())
    write_map[fd] = RESPONSE;

  bytes_written = write(fd, write_map[fd].c_str(), write_map[fd].size());
  if (bytes_written <= 0) {
    epoll_remove(fd);
    write_map.erase(fd);
    close(fd);
  }

  write_map[fd].erase(0, bytes_written);
  if (write_map[fd].empty()) {
    epoll_remove(fd);
    write_map.erase(fd);
    close(fd);
  }
}

int Server::numListeningSockets() {
  int cnt = 0;

  for (size_t i = 0; i < conf.getServerConfigs().size(); i++)
    cnt += conf.getServerConfigs()[i].getIPs().size();

  return cnt;
}

unsigned int Server::IPToNum(std::string ip) {
  std::stringstream ss(ip);
  unsigned int a, b, c, d;
  char dot;

  ss >> a >> dot >> b >> dot >> c >> dot >> d;

  return a << 24 | b << 16 | c << 8 | d;
}
