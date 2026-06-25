#include "server/Server.hpp"
#include "parser/config/ConfigParser.hpp"
#include "parser/config/ServerConfig.hpp"
#include "request/HttpRequest.hpp"
#include "response/HttpResponse.hpp"
#include "server/Client.hpp"
#include "server/Socket.hpp"
#include "utils.hpp"
#include <cerrno>
#include <csignal>
#include <cstring>
#include <exception>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define RESET   "\033[0m"

volatile sig_atomic_t Server::isRunning = 1;

Server::Server(const ConfigParser &configParser) : globalConfig(configParser) {
  signal(SIGINT, Server::sigintHandler);
}

void Server::sigintHandler(int signum) {
  (void)signum;
  isRunning = 0;
}

Server::~Server() {
  for (size_t i = 0; i < Sockets.size(); i++) {
    delete Sockets[i];
  }
  Sockets.clear();
  for (std::map<int, Client *>::iterator it = clientMap.begin();
       it != clientMap.end(); ++it) {
    delete it->second;
  }
  clientMap.clear();
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
  while (Server::isRunning) {
    handleEvents(epoll.wait());
    sweepTimeouts();
  }
}

void Server::handleEvents(int n) {
  if (n > 0)
  for (int i = 0; i < n; i++)
    handleSingleEvent(epoll.getEventsFd(i), epoll.getEventsMask(i));
}

void Server::handleSingleEvent(int triggeredFd, uint32_t eventsMask)
{
  epoll.printRegistered();
	std::cout <<RED<<"-----------------\nTRIGGERED_FD: [" << triggeredFd << "]\n-----------------\n"<<RESET;
  if (clientMap.find(triggeredFd) != clientMap.end()) {
	std::cout <<GREEN<<"Found in CLIENT MAP :"<<triggeredFd<<"\n-----------------\n"<<RESET;
    if (clientMap[triggeredFd]->handleEvent(eventsMask) == false)
	{
      //epoll.remove(triggeredFd);
      deleteMapItem<std::map<int, Client *> >(clientMap, triggeredFd);
    }
  } else if (cgiMap.find(triggeredFd) != cgiMap.end()) {
	std::cout <<BLUE<<"Found in CGI MAP: "<<triggeredFd<<"\n-----------------\n"<<RESET;
    // TODO: cgi handle
    Cgi *cgi = cgiMap[triggeredFd];
    if(cgi->handleEvent())
    {
      int targetFd = cgi->getClientFd();
      if(clientMap.count(targetFd)) {
         clientMap[targetFd]->setResponse(cgi->getOutput());
         epoll.modWrite(targetFd);
      }
      epoll.remove(triggeredFd);
      close(triggeredFd);
      cgiMap.erase(triggeredFd);
      delete cgi;
    }
    //clientMap.erase(triggeredFd);
  } else {
    handleServer(triggeredFd);
  }
}

void Server::handleServer(int serverFd) {
  int clientFd;
  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);
  std::cout << "ACCEPT REQUEST FROM SERVER [" << serverFd << "]\n";
  clientFd = accept(serverFd, (struct sockaddr *)&addr, &addr_len);
  if (clientFd < 0) {
    switch (errno) {
    case EMFILE:
    case ENFILE:
      //std::cout << "fd limit reached" << std::endl;
    case EAGAIN:
      return;
    default:
      throw std::runtime_error("accept 103: " + std::string(strerror(errno)));
    }
  }
  try {
    clientMap[clientFd] =
        new Client(clientFd, *this, getServerConfig(serverFd));
  } catch (std::exception &e) {
    close(clientFd);
    throw;
  }
  return;
}

const ServerConfig &Server::getServerConfig(int serverFd) const {
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

void Server::sweepTimeouts() {
  std::map<int, Client *>::iterator it = clientMap.begin();
  while (it != clientMap.end()) {
    Client *client = it->second;

    if (client->isTimedOut()) {
      delete client;
      clientMap.erase(it++);
    } else {
      ++it;
    }
  }
}

Epoll &Server::getEpoll() { return epoll; }

std::map<int, Cgi *> &Server::getCgiMap() { return cgiMap; }
