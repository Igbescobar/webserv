#include "server/Client.hpp"
#include "cgi/Cgi.hpp"
#include "parser/config/ServerConfig.hpp"
#include "request/HttpRequest.hpp"
#include "response/HttpResponse.hpp"
#include "server/Socket.hpp"
#include "utils.hpp"
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <unistd.h>

Client::Client(int clientSocket, Server &server,
               const ServerConfig &serverConfig)
    : clientFd(clientSocket), serverConfig(serverConfig), server(server),
      cgi(NULL) {
  request = HttpRequest(serverConfig);
  setNonBlocking(clientSocket);
  setCloseOnExec(clientSocket);
  server.getEpoll().addRead(clientSocket);
  connectionStart = lastActivity = std::time(NULL);
  requestSize = 0;
}

Client::~Client() { close(clientFd); }

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

  t_state requestState = request.getState();

  // TODO: not readable code
  if (requestState == INCOMPLETE)
    return true;
  else if (requestState == COMPLETE) {
    HttpResponse response(serverConfig, request);
    if (response.isCgi()) {
      // TODO
      cgi = new Cgi(server, response.getCgiPath());
    } else {
      responseStr = response.getResponse();
    }
    server.getEpoll().modWrite(clientFd);
    return true;
  } else if (requestState == ERROR) {
    responseStr =
        HttpResponse(serverConfig, request.getErrorCode()).getResponse();
    server.getEpoll().modWrite(clientFd);
    return true;
  } else {
    throw std::runtime_error("unknown request state");
  }
}

bool Client::write(int clientFd) {
  if (responseStr.empty()) {
    if (cgi->getState() == INCOMPLETE)
      return true;
    responseStr = HttpResponse(serverConfig, cgi->getOutput()).getResponse();
  }

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
