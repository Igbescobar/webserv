#include "server/Client.hpp"
#include "parser/config/ServerConfig.hpp"
#include "request/HttpRequest.hpp"
#include "response/HttpResponse.hpp"
#include "server/Socket.hpp"
#include <iostream>
#include <stdexcept>
#include <unistd.h>

Client::Client(int clientSocket, Epoll &epoll, const ServerConfig &serverConfig)
    : clientFd(clientSocket), epoll(epoll), serverConfig(serverConfig) {
  request = HttpRequest(serverConfig);
  Socket::setNonBlocking(clientSocket);
  epoll.addRead(clientSocket);
}

Client::~Client() { close(clientFd); }

bool Client::handleEvent(uint32_t eventsMask) {
  if (eventsMask & EPOLLIN)
    return read(clientFd);
  else if (eventsMask & EPOLLOUT)
    return write(clientFd);
  return false;
}

bool Client::read(int clientFd) {
  char buffer[BUF_SIZE + 1];
  int bytesRead;

  bytesRead = ::read(clientFd, buffer, BUF_SIZE);
  if (bytesRead <= 0) {
    epoll.remove(clientFd);
    return false;
  }

  buffer[bytesRead] = '\0';
  request.append(buffer);

  switch (request.getState()) {
  case INCOMPLETE:
    return true;
  case COMPLETE:
    responseStr = HttpResponse(serverConfig, request).getResponse();
    epoll.modWrite(clientFd);
    return true;
  case ERROR:
    responseStr =
        HttpResponse(serverConfig, request.getErrorCode()).getResponse();
    epoll.modWrite(clientFd);
    return true;
  default:
    throw std::runtime_error("unknown request state");
  }
}

bool Client::write(int clientFd) {
  int bytesWritten;

  bytesWritten = ::write(clientFd, responseStr.c_str(), responseStr.size());
  if (bytesWritten <= 0) {
    epoll.remove(clientFd);
    return false;
  }

  responseStr.erase(0, bytesWritten);

  if (responseStr.empty()) {
    epoll.remove(clientFd);
    return false;
  }
  return true;
}
