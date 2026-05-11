#include "server/Server.hpp"
#include "parser/config/ConfigParser.hpp"
#include "parser/config/ServerConfig.hpp"
#include "request/HttpRequest.hpp"
#include "response/HttpResponse.hpp"
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

Server::Server(const ConfigParser &configParser) : globalConfig(configParser) {}

Server::~Server() {}

void Server::startAllServers() {
  const ServerConfig *serverConfigPtr;

  for (size_t i = 0; i < globalConfig.getServerConfigs().size(); i++) {
    serverConfigPtr = &globalConfig.getServerConfigs()[i];
    for (size_t j = 0; j < serverConfigPtr->getIPs().size(); j++) {
      startSingleServer(serverConfigPtr->getIPs()[j],
                        serverConfigPtr->getPorts()[j]);
    }
  }
}

void Server::startSingleServer(std::string ip, int port) {
  serverFds.push_back(socketCreate());

  int last = serverFds.size() - 1;

  setNonBlocking(serverFds[last]);
  socketBind(serverFds[last], ip, port);
  socketListen(serverFds[last]);
  epoll.addRead(serverFds[last]);
}

void Server::run() {
  startAllServers();
  while (true) {
    handleEvents(epoll.wait());
  }
}

void Server::handleEvents(int n) {
  for (int i = 0; i < n; i++)
    handleSingleEvent(epoll.getEventsFd(i), epoll.getEventsMask(i));
}

void Server::handleSingleEvent(int fd, uint32_t eventsMask) {
  for (size_t i = 0; i < serverFds.size(); i++) {
    if (fd == serverFds[i]) {
      handleServer(serverFds[i]);
      return;
    }
  }
  handleClient(fd, eventsMask);
}

void Server::handleServer(int fd) {
  int new_socket;
  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);

  new_socket = accept(fd, (struct sockaddr *)&addr, &addr_len);
  if (new_socket < 0)
    throw std::runtime_error("accept: " + std::string(strerror(errno)));

  setNonBlocking(new_socket);
  requestMap[new_socket] = HttpRequest(getServerConfig(fd));
  epoll.addRead(new_socket);
  return;
}

void Server::handleClient(int fd, uint32_t eventsMask) {
  if (eventsMask & EPOLLIN)
    clientRead(fd);
  else if (eventsMask & EPOLLOUT)
    clientWrite(fd);
}

void Server::clientRead(int fd) {
  char buffer[BUF_SIZE + 1];
  int bytes_read;

  bytes_read = read(fd, buffer, BUF_SIZE);
  if (bytes_read <= 0) {
    epoll.remove(fd);
    requestMap.erase(fd);
    close(fd);
    return;
  }

  buffer[bytes_read] = '\0';
  requestMap[fd].append(buffer);

  switch (requestMap[fd].getState()) {
  case INCOMPLETE:
    return;
  case COMPLETE:
    responseMap[fd] =
        HttpResponse(requestMap[fd].getServerConfig(), requestMap[fd]);
    break;
  case ERROR:
    responseMap[fd] = HttpResponse(requestMap[fd].getServerConfig(),
                                   requestMap[fd].getErrorCode());
    break;
  default:
    throw std::runtime_error("undefined t_state value");
  }

  requestMap.erase(fd);

  epoll.modWrite(fd);
}

void Server::clientWrite(int fd) {
  int bytes_written = write(fd, responseMap[fd].getResponse().c_str(),
                            responseMap[fd].getResponse().size());

  if (bytes_written <= 0) {
    epoll.remove(fd);
    responseMap.erase(fd);
    close(fd);
  }

  responseMap[fd].erase(bytes_written);

  if (responseMap[fd].empty()) {
    epoll.remove(fd);
    responseMap.erase(fd);
    close(fd);
  }
}

int Server::numListeningSockets() {
  int cnt = 0;

  for (size_t i = 0; i < globalConfig.getServerConfigs().size(); i++)
    cnt += globalConfig.getServerConfigs()[i].getIPs().size();

  return cnt;
}

unsigned int Server::IPToNum(std::string ip) {
  std::stringstream ss(ip);
  unsigned int a, b, c, d;
  char dot;

  ss >> a >> dot >> b >> dot >> c >> dot >> d;

  return a << 24 | b << 16 | c << 8 | d;
}

void Server::printServersFds() {
  for (size_t i = 0; i < serverFds.size(); i++) {
    std::cout << serverFds[i] << " ";
  }
  std::cout << std::endl;
}

ServerConfig Server::getServerConfig(int serverFd) {
  const ServerConfig *serverConfigPtr;
  int idx = 0;

  for (size_t i = 0; i < globalConfig.getServerConfigs().size(); i++) {
    serverConfigPtr = &(globalConfig.getServerConfigs()[i]);
    for (size_t j = 0; j < serverConfigPtr->getIPs().size(); j++) {
      if (serverFd == serverFds[idx]) {
        return *serverConfigPtr;
      }
      idx++;
    }
  }
  throw std::runtime_error("ServerConfig not found");
}
