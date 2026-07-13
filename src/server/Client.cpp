#include "server/Client.hpp"
#include "cgi/Cgi.hpp"
#include "parser_config/ServerConfig.hpp"
#include "request/HttpRequest.hpp"
#include "response/CgiTargetResolver.hpp"
#include "response/ErrorResponseBuilder.hpp"
#include "response/HttpResponse.hpp"
#include "response/ResponseHandler.hpp"
#include "server/Socket.hpp"
#include "utils.hpp"
#include <ctime>
#include <fcntl.h>
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
  if (bytesRead <= 0 || requestSize > REQUEST_LIMIT)
  {
    std::cerr << "[READ] closing: bytesRead=" << bytesRead << " errno=" << errno << "\n";
    return false;
  }
  if (requestSize > REQUEST_LIMIT) {
        std::cerr << "[READ] closing: REQUEST_LIMIT exceeded size=" << requestSize << "\n";
        return false;
  }
  buffer[bytesRead] = '\0';
  request.append(std::string(buffer, bytesRead));
  handleRequestState(request.getState());
  return true;
}

void Client::handleRequestState(t_state state) {
  CgiTarget target = CgiTargetResolver::resolve(serverConfig, request);
  if (state == COMPLETE) {
      std::cout<<"request body: \n"<<request.getBody()<<"\n";
      responseStr =
          ResponseHandler(serverConfig, request).handle().getResponse();
      server.getEpoll().modWrite(clientFd);
  } else if (state == ERROR) {
    responseStr = ResponseHandler(serverConfig, request).handle().getResponse();
    server.getEpoll().modWrite(clientFd);
  } else if (state == INCOMPLETE)
  {
    if (target.isCgi) {
      if(!cgi)
      {
        std::cout<<"AQUI YA DETECTA QUE ES CGI\n";
        cgi = new Cgi(request, *target.location, clientFd);
        if (cgi->getState() == ERROR) {
          responseStr = ErrorResponseBuilder::build(serverConfig, request,
                                                  cgi->getErrorCode())
                          .getResponse();
          delete cgi;
          cgi = NULL;
          server.getEpoll().modWrite(clientFd);
        }
      }
      if(cgi->getState() == COMPLETE)
      {
         std::cout<<"CGI EXECUTES\n";
         cgi->execute(server);
      }
    }
    //std::cerr << "[READ] request complete, request body =\n" <<request.getRequest();
    return;
  }
  else
    throw std::runtime_error("unknown request state");
}

bool Client::write(int clientFd) {
  int bytesWritten = ::write(clientFd, responseStr.c_str(), responseStr.size());
  if (bytesWritten <= 0)
    return false;

  responseStr.erase(0, bytesWritten);

  if (!responseStr.empty())
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

void Client::setResponse(const std::string &response) {
  responseStr = response;
}

void Client::sendError(int code) {
  responseStr = ErrorResponseBuilder::build(serverConfig, code).getResponse();
  server.getEpoll().modWrite(clientFd);
}

const ServerConfig &Client::getServerConfig() const { return serverConfig; }

const HttpRequest &Client::getRequest() const { return request; }
