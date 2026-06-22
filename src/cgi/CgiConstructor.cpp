#include "cgi/CgiConstructor.hpp"
#include "request/HttpRequest.hpp"
#include "server/Client.hpp"
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <unistd.h>

CgiConstructor::CgiConstructor(Server &server, std::string path)
    : server(server), path(path), state(INCOMPLETE) {
  if (pipe(pipefd) < 0)
    throw std::runtime_error("pipe: " + std::string(strerror(errno)));

  write(pipefd[1], CGI_SAMPLE_OUTPUT, sizeof(CGI_SAMPLE_OUTPUT));

  //server.getCgiMap()[pipefd[0]] = this;
  server.getEpoll().addRead(pipefd[0]);
}

void CgiConstructor::handleEvent() {
  char buf[BUF_SIZE + 1];

  int bytesRead = read(pipefd[0], buf, BUF_SIZE);
  buf[bytesRead] = '\0';
  output += buf;
  state = COMPLETE;
}

std::string CgiConstructor::getOutput() { return output; }

t_state CgiConstructor::getState() { return state; }
