#include "server/Client.hpp"
#include "parser_config/ServerConfig.hpp"
#include "request/HttpRequest.hpp"
#include "response/HttpResponse.hpp"
#include "response/ResponseHandler.hpp"
#include "server/Socket.hpp"
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <unistd.h>

Client::Client(int clientSocket, Epoll &epoll, const ServerConfig &serverConfig)
    : clientFd(clientSocket), epoll(epoll), serverConfig(serverConfig) {
  request = HttpRequest(serverConfig);
  Socket::setNonBlocking(clientSocket);
  epoll.addRead(clientSocket);
  connectionStart = lastActivity = std::time(NULL);
  requestSize = 0;
}

Client::~Client() {
  close(clientFd);
  epoll.remove(clientFd);
}

bool Client::handleEvent(uint32_t eventsMask) {
  updateActivity();
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
  requestSize += bytesRead;
  if (bytesRead <= 0 || requestSize > REQUEST_LIMIT) {
    return false;
  }

  buffer[bytesRead] = '\0';
  request.append(buffer);

  switch (request.getState()) {
  case INCOMPLETE:
    return true;
  case COMPLETE: {
    HttpResponse resp = ResponseHandler(serverConfig, request).handle();
    responseStr = resp.getResponse();
    epoll.modWrite(clientFd);
    return true;
  }
  case ERROR: {
    HttpResponse resp = ResponseHandler(serverConfig, request).handle();
    responseStr = resp.getResponse();
    epoll.modWrite(clientFd);
    return true;
  }
  default:
    throw std::runtime_error("unknown request state");
  }
}

bool Client::write(int clientFd) {
  int bytesWritten;

  bytesWritten = ::write(clientFd, responseStr.c_str(), responseStr.size());
  if (bytesWritten <= 0) {
    return false;
  }

  responseStr.erase(0, bytesWritten);

  if (responseStr.empty()) {
    return false;
  }
  return true;
}

void Client::updateActivity() { lastActivity = std::time(NULL); }

bool Client::isTimedOut() {
  time_t currentTime = std::time(NULL);
  if ((currentTime - lastActivity) > IDLE_LIMIT)
    return true;
  if ((currentTime - connectionStart) > ABSOLUTE_LIMIT)
    return true;
  return false;
}
