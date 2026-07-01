#include "server/Client.hpp"
#include "cgi/Cgi.hpp"
#include "parser/config/ServerConfig.hpp"
#include "request/HttpRequest.hpp"
#include "response/HttpResponse.hpp"
#include "server/Socket.hpp"
#include "utils.hpp"
#include <ctime>
#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include <sys/stat.h>
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

bool Client::isCGI() {
  std::string method = request.getMethod();
  std::string uri = request.getUri();
  size_t dotPos = uri.find_last_of('.');
  std::cout << uri << "\n";
  if (dotPos == std::string::npos) {
    std::cout << "Does not have extension\n";
    return false;
  }
  size_t questionpos = uri.find('?', dotPos);
  std::string ext = uri.substr(dotPos, questionpos - dotPos);
  const std::vector<LocationConfig> &locations = serverConfig.getLocations();
  for (size_t i = 0; i < locations.size(); i++) {
    const std::vector<std::string> &exts = locations[i].getCgiPassExtensions();
    for (size_t j = 0; j < exts.size(); j++) {
      std::cout << exts[j] << "\n";
      if (exts[j] == ext)
        return true;
    }
  }
  return false;
}

const LocationConfig *Client::getMatchingLocation(const std::string &uri) {
  const std::vector<LocationConfig> &locations = serverConfig.getLocations();
  size_t secondSlash = uri.find('/', 1);
  std::string segment = uri.substr(0, secondSlash);

  for (size_t i = 0; i < locations.size(); i++) {
    if (locations[i].getPattern() == segment)
      return &locations[i];
  }
  return NULL;
}

bool Client::read(int clientFd) {
  char buffer[BUF_SIZE + 1];
  int bytesRead;

  bytesRead = ::read(clientFd, buffer, BUF_SIZE);
  requestSize += bytesRead;
  if (bytesRead <= 0 || requestSize > REQUEST_LIMIT)
    return false;

  buffer[bytesRead] = '\0';
  request.append(buffer);

  handleRequestState(request.getState());
  return true;
}

void Client::handleRequestState(t_state state) {
  if (state == COMPLETE) {
    HttpResponse response(serverConfig, request);
    if (isCGI()) {
      const LocationConfig *loc = getMatchingLocation(request.getUri());
      if (loc == NULL) {
        responseStr = response.getResponse();
        server.getEpoll().modWrite(clientFd);
        return;
      }
      cgi = new Cgi(request, *loc, clientFd);
      cgi->execute(server);
    } else {
      responseStr = response.getResponse();
      server.getEpoll().modWrite(clientFd);
    }
  } else if (state == ERROR) {
    responseStr =
        HttpResponse(serverConfig, request.getErrorCode()).getResponse();
    server.getEpoll().modWrite(clientFd);
  } else
    throw std::runtime_error("unknown request state");
}

bool Client::write(int clientFd) {
  int bytesWritten = ::write(clientFd, responseStr.c_str(), responseStr.size());
  if (bytesWritten <= 0) {
    return false;
  }

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

/*void printFds()
{
      for (int i = 0; i < 20; i++) {
        struct stat st;
        if (fstat(i, &st) == 0) {
        std::cerr << "  fd=" << i;
        if (S_ISSOCK(st.st_mode)) std::cerr << " SOCKET";
        else if (S_ISFIFO(st.st_mode)) std::cerr << " PIPE";
        else if (S_ISREG(st.st_mode)) std::cerr << " FILE";
        std::cerr << "\n";
    }
}
}*/
