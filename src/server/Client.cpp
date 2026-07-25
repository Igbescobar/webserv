#include "server/Client.hpp"
#include "cgi/Cgi.hpp"
#include "parser_config/ServerConfig.hpp"
#include "request/HttpRequest.hpp"
#include "response/CgiResponder.hpp"
#include "response/CgiTargetResolver.hpp"
#include "response/ErrorResponseBuilder.hpp"
#include "response/HttpResponse.hpp"
#include "response/ResponseHandler.hpp"
#include "server/Socket.hpp"
#include "utils.hpp"
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <unistd.h>

Client::Client(int clientSocket, Server &server,
               const ServerConfig &serverConfig)
    : clientFd(clientSocket), serverConfig(serverConfig), server(server),
      cgi(NULL), bytesSent(0) {
  request = HttpRequest(serverConfig);
  setNonBlocking(clientSocket);
  setCloseOnExec(clientSocket);
  server.getEpoll().addRead(clientSocket);
  connectionStart = lastActivity = std::time(NULL);
  requestSize = 0;
  reference = 0;
}

Client::~Client() {
  if (cgi) {
    delete cgi;
    cgi = NULL;
  }
  server.getEpoll().remove(clientFd);
  close(clientFd);
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
  if (bytesRead <= 0)
    return false;
  requestSize += bytesRead;
  int percentage = requestSize / 1000000;
  if (percentage % 10 == 0 && percentage != reference) {
    reference = percentage;
    std::cerr << percentage << "%" << std::endl;
  }
  if (requestSize > REQUEST_LIMIT)
    return false;

  buffer[bytesRead] = '\0';
  request.append(std::string(buffer, bytesRead));

  handleRequestState(request.getState());
  return true;
}

void Client::handleRequestState(t_state state) {
  if (state == COMPLETE) {
    CgiTarget target = CgiTargetResolver::resolve(serverConfig, request);
    if (target.isCgi) {
      try {
        cgi = new Cgi(server, target.scriptPath, request);
      } catch (...) {
        responseStr = ErrorResponseBuilder::build(serverConfig, request,
                                                  INTERNAL_SERVER_ERROR)
                          .getResponse();
        cgi = NULL;
      }
      server.getEpoll().modWrite(clientFd);
    } else {
      responseStr =
          ResponseHandler(serverConfig, request).handle().getResponse();
      server.getEpoll().modWrite(clientFd);
    }
  } else if (state == ERROR) {
    responseStr = ResponseHandler(serverConfig, request).handle().getResponse();
    server.getEpoll().modWrite(clientFd);
  } else if (state == INCOMPLETE) {
    return;
  } else {
    throw std::runtime_error("unknown request state");
  }
}

bool Client::write(int clientFd) {
  if (cgi) {
    if (cgi->getState() == INCOMPLETE)
      return true;
    responseStr = CgiResponder::handle(serverConfig, request, cgi->getOutput())
                      .getResponse();
    delete cgi;
    cgi = NULL;
  }

  int bytesWritten = ::write(clientFd, responseStr.c_str() + bytesSent,
                             responseStr.size() - bytesSent);
  bytesSent += bytesWritten;
  if (bytesWritten <= 0) {
    return false;
  }

  if (bytesSent == responseStr.size())
    return true;

  return false;
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
