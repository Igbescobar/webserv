#include "server/Server.hpp"
#include "parser/config/ConfigParser.hpp"
#include "parser/config/ServerConfig.hpp"
#include "request/HttpRequest.hpp"
#include "response/HttpResponse.hpp"
#include "server/Client.hpp"
#include "server/Socket.hpp"
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

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

void Server::handleSingleEvent(int triggeredFd, uint32_t eventsMask) {
  if (clientMap.find(triggeredFd) != clientMap.end()) {
    if (clientMap[triggeredFd]->handleEvent(eventsMask) == false)
      delete clientMap[triggeredFd];
  } else {
    handleServer(triggeredFd);
  }
}

void Server::handleServer(int serverFd) {
  int clientFd;
  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);

  // TODO: check errno
  clientFd = accept(serverFd, (struct sockaddr *)&addr, &addr_len);
  if (clientFd < 0)
    throw std::runtime_error("accept: " + std::string(strerror(errno)));

  clientMap[clientFd] = new Client(clientFd, epoll, getServerConfig(serverFd));
  return;
}

// TODO: fix this function
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
