#include "cgi/Cgi.hpp"
#include "request/HttpRequest.hpp"
#include "server/Client.hpp"
#include "utils.hpp"
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>

Cgi::Cgi(Server &server, std::string path, HttpRequest &req)
    : server(server), path(path), req(req), state(INCOMPLETE) {
  if (pipe(pipefd) < 0)
    throw std::runtime_error("pipe: " + std::string(strerror(errno)));

  server.getCgiMap()[pipefd[0]] = this;
  server.getEpoll().addRead(pipefd[0]);

  pid_t pid = fork();
  if (pid < 0) {
    throw std::runtime_error("fork: " + std::string(strerror(errno)));
  } else if (pid == 0) {
    // set argv
    char *argv[] = {const_cast<char *>("sample"), NULL};

    // set envp
    std::vector<std::string> env = buildEnv();
    std::vector<char *> envp;
    for (size_t i = 0; i < env.size(); i++) {
      envp.push_back(const_cast<char *>(env[i].c_str()));
    }
    envp.push_back(NULL);

    // chdir
    std::string dirPath = path.substr(0, path.find_last_of('/')).c_str();
    if (chdir(path.substr(0, path.find_last_of('/')).c_str()) < 0)
      exit(127); // TODO: how to handle this?
    std::string scriptName = path.substr(path.find_last_of('/') + 1);

    // set io
    close(pipefd[0]);
    dup2(pipefd[1], 1);
    close(pipefd[1]);

    // run execve
    execve(scriptName.c_str(), argv, envp.data());
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

// TODO: missing variables
std::vector<std::string> Cgi::buildEnv() {
  std::vector<std::string> env;
  env.push_back("REQUEST_METHOD=" + req.getMethod());
  env.push_back("CONTENT_LENGTH=" + req.getHeader("content-length"));
  env.push_back("CONTENT_TYPE=" + req.getHeader("content-type"));
  env.push_back("QUERY_STRING=" + extractQuery());
  return env;
}

std::string Cgi::extractQuery() {
  size_t queryPos = req.getUri().find('?');

  if (queryPos != std::string::npos)
    return req.getUri().substr(queryPos + 1);
  else
    return "";
}
