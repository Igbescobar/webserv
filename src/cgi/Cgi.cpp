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
    : server(server), path(path), _req(req), state(INCOMPLETE),
      reqBody(req.getBody()) {
  if (pipe(outputPipe) < 0)
    throw std::runtime_error("pipe: " + std::string(strerror(errno)));
  if (req.getMethod() == "POST") {
    if (pipe(bodyPipe) < 0)
      throw std::runtime_error("pipe: " + std::string(strerror(errno)));
    cgiState = SENDING_BODY;
    server.getCgiMap()[bodyPipe[1]] = this;
    server.getEpoll().addWrite(bodyPipe[1]);
  } else {
    cgiState = READING_OUTPUT;
    server.getCgiMap()[outputPipe[0]] = this;
    server.getEpoll().addRead(outputPipe[0]);
  }

  childPid = fork();
  if (childPid < 0) {
    throw std::runtime_error("fork: " + std::string(strerror(errno)));
  } else if (childPid == 0) {
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

    // set input
    if (req.getMethod() == "POST") {
      close(bodyPipe[1]);
      dup2(bodyPipe[0], 0);
      close(bodyPipe[0]);
    }

    // set output
    close(outputPipe[0]);
    dup2(outputPipe[1], 1);
    close(outputPipe[1]);

    // run execve
    execve(scriptName.c_str(), argv, envp.data());
    std::cerr << "execve: " << strerror(errno) << std::endl;
    exit(127);
  }
  if (req.getMethod() == "POST")
    close(bodyPipe[0]);
  close(outputPipe[1]);
}

// TODO: may not read the whole thing in one call
void Cgi::readingOutput() {
  char buf[BUF_SIZE + 1];

  int bytesRead = ::read(outputPipe[0], buf, BUF_SIZE);
  if (bytesRead < 0) {
    throw std::runtime_error(__FUNCTION__);
  } else if (bytesRead == 0) {
    state = COMPLETE;
    server.getEpoll().remove(outputPipe[0]);
    close(outputPipe[0]);
  }
  buf[bytesRead] = '\0';
  output += buf;
}

void Cgi::sendingBody() {
  int bytesWritten = ::write(bodyPipe[1], reqBody.c_str(), reqBody.size());
  if (bytesWritten <= 0) {
    throw std::runtime_error(__FUNCTION__);
  }
  reqBody.erase(0, bytesWritten);

  // TODO: check leaks
  if (reqBody.empty()) {
    server.getEpoll().remove(bodyPipe[1]);
    close(bodyPipe[1]);
    cgiState = READING_OUTPUT;
    server.getEpoll().addRead(outputPipe[0]);
    server.getCgiMap().erase(bodyPipe[1]);
    server.getCgiMap()[outputPipe[0]] = this;
  }
}

void Cgi::handleEvent() {
  if (cgiState == SENDING_BODY) {
    sendingBody();
  } else if (cgiState == READING_OUTPUT) {
    readingOutput();
  } else {
    throw std::runtime_error("unknown cgiState");
  }
}

std::string Cgi::getOutput() { return output; }

t_state Cgi::getState() { return state; }

// TODO: missing variables
std::vector<std::string> Cgi::buildEnv() {
  std::vector<std::string> env;
  env.push_back("REQUEST_METHOD=" + _req.getMethod());
  env.push_back("CONTENT_LENGTH=" + _req.getHeader("content-length"));
  env.push_back("CONTENT_TYPE=" + _req.getHeader("content-type"));
  env.push_back("QUERY_STRING=" + extractQuery());
  return env;
}

std::string Cgi::extractQuery() {
  size_t queryPos = _req.getUri().find('?');

  if (queryPos != std::string::npos)
    return _req.getUri().substr(queryPos + 1);
  else
    return "";
}
