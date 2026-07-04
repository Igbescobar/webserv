#include "cgi/Cgi.hpp"
#include "request/HttpRequest.hpp"
#include "server/Client.hpp"
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>

Cgi::Cgi(Server &server, std::string path)
    : server(server), path(path), state(INCOMPLETE) {
  if (pipe(pipefd) < 0)
    throw std::runtime_error("pipe: " + std::string(strerror(errno)));

  server.getCgiMap()[pipefd[0]] = this;
  server.getEpoll().addRead(pipefd[0]);

  pid_t pid = fork();
  if (pid < 0) {
    throw std::runtime_error("fork: " + std::string(strerror(errno)));
  } else if (pid == 0) {
    close(pipefd[0]);
    dup2(pipefd[1], 1);
    close(pipefd[1]);
    char *argv[] = {const_cast<char *>("sample"), NULL};
    char *envp[] = {NULL};
    execve(path.c_str(), argv, envp);
    std::cerr << "execve: " << strerror(errno) << std::endl;
    exit(127);
  }
  close(pipefd[1]);
}

void Cgi::handleEvent() {
  char buf[BUF_SIZE + 1];

  int bytesRead = read(pipefd[0], buf, BUF_SIZE);
  buf[bytesRead] = '\0';
  output += buf;
  state = COMPLETE;
  server.getEpoll().remove(pipefd[0]);
  close(pipefd[0]);
}

std::string Cgi::getOutput() { return output; }

t_state Cgi::getState() { return state; }
