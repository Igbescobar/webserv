#include "server/Server.hpp"
#include "parser/config/ConfigParser.hpp"
#include "parser/config/ServerConfig.hpp"
#include "request/HttpRequest.hpp"
#include "response/HttpResponse.hpp"
#include "server/Socket.hpp"
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

Server::Server(const ConfigParser &configParser) : globalConfig(configParser) {}

Server::~Server() {
  for (size_t i = 0; i < Sockets.size(); i++) {
    delete Sockets[i];
  }
  Sockets.clear();
}

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
  Sockets.push_back(new Socket(ip, port));
  epoll.addRead(Sockets.back()->getFd());
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
  for (size_t i = 0; i < Sockets.size(); i++) {
    if (fd == Sockets[i]->getFd()) {
      handleServer(Sockets[i]->getFd());
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

  Socket::setNonBlocking(new_socket);
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

ServerConfig Server::getServerConfig(int serverFd) {
  const ServerConfig *serverConfigPtr;
  int idx = 0;

  for (size_t i = 0; i < globalConfig.getServerConfigs().size(); i++) {
    serverConfigPtr = &(globalConfig.getServerConfigs()[i]);
    for (size_t j = 0; j < serverConfigPtr->getIPs().size(); j++) {
      if (serverFd == Sockets[idx]->getFd()) {
        return *serverConfigPtr;
      }
      idx++;
    }
  }
  throw std::runtime_error("ServerConfig not found");
}
