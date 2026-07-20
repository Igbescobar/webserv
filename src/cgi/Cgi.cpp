#include "cgi/Cgi.hpp"
#include "request/HttpRequest.hpp"
#include "server/Client.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cctype>
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

Cgi::Cgi(Server &server, std::string path, HttpRequest &req)
    : server(server), path(path), _req(req), state(INCOMPLETE),
      reqBody(req.getBody()), _bodyBytesSent(0) {

  std::cerr << "path: " << path << std::endl;
  interpreter = getInterpreter(extractExtension());
  std::cerr << "interpreter: " << interpreter << std::endl;

  if (pipe(outputPipe) < 0)
    throw std::runtime_error("pipe: " + std::string(strerror(errno)));
  if (pipe(bodyPipe) < 0)
    throw std::runtime_error("pipe: " + std::string(strerror(errno)));
  std::cerr << "outputPipe: " << outputPipe[0] << " " << outputPipe[1]
            << std::endl;
  std::cerr << "bodyPipe: " << bodyPipe[0] << " " << bodyPipe[1] << std::endl;
  setNonBlocking(outputPipe[0]);
  setNonBlocking(outputPipe[1]);
  setNonBlocking(bodyPipe[0]);
  setNonBlocking(bodyPipe[1]);
  if (req.getMethod() == "POST") {
    server.getCgiMap()[bodyPipe[1]] = this;
    server.getEpoll().addWrite(bodyPipe[1]);
  }
  server.getCgiMap()[outputPipe[0]] = this;
  server.getEpoll().addRead(outputPipe[0]);

  childPid = fork();
  if (childPid < 0) {
    throw std::runtime_error("fork: " + std::string(strerror(errno)));
  } else if (childPid == 0) {

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

    // set argv
    // char *argv[] = {const_cast<char *>("sample"), NULL};
    char *argv[] = {const_cast<char *>(interpreter.c_str()),
                    const_cast<char *>(scriptName.c_str()), NULL};

    // set input
    close(bodyPipe[1]);
    dup2(bodyPipe[0], 0);
    close(bodyPipe[0]);

    // set output
    close(outputPipe[0]);
    dup2(outputPipe[1], 1);
    close(outputPipe[1]);

    // run execve
    // execve(scriptName.c_str(), argv, envp.data());
    std::cerr << "envp: ";
    printVector(envp);
    execve(argv[0], argv, envp.data());
    // TODO: what to do if it fails?
    std::cerr << "execve: " << strerror(errno) << std::endl;
    exit(127);
  }
  if (req.getMethod() == "GET") {
    close(bodyPipe[1]);
    bodyPipe[1] = -1;
  }
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
    server.getCgiMap().erase(outputPipe[0]);
    close(outputPipe[0]);
    outputPipe[0] = -1;
  }
  buf[bytesRead] = '\0';
  output += buf;
  if (state == COMPLETE) {
    // std::cerr << "=== cgi output ===" << std::endl << output << std::endl;
  }
}

void Cgi::sendingBody() {
  // std::cerr << __FUNCTION__ << std::endl;
  // int bytesWritten = ::write(bodyPipe[1], reqBody.c_str(), reqBody.size());
  // int bytesWritten = ::write(bodyPipe[1], reqBody.c_str(), 100);
  // int bytesWritten = ::write(bodyPipe[1], reqBody.c_str() + _bodyBytesSent,
  //                            reqBody.size() - _bodyBytesSent);
  size_t remaining = reqBody.size() - _bodyBytesSent;
  size_t chunkSize = remaining < 10000 ? remaining : 10000;
  int bytesWritten =
      ::write(bodyPipe[1], reqBody.c_str() + _bodyBytesSent, chunkSize);
  // std::cerr << "after write" << std::endl;
  if (bytesWritten <= 0) {
    throw std::runtime_error("write: " + std::string(strerror(errno)));
  } else {
    _bodyBytesSent += bytesWritten;
  }
  // std::cerr << "_bodyBytesSent: " << _bodyBytesSent << std::endl;
  // std::cerr << "reqBody.size(): " << reqBody.size() << std::endl;
  // std::cerr << "=== sent" << std::endl;
  // std::cerr << reqBody.substr(0, bytesWritten) << std::endl;
  // std::cerr << "===" << std::endl;
  // std::cerr << "remaining: " << reqBody.size() - _bodyBytesSent << std::endl;
  // reqBody.erase(0, bytesWritten);

  // TODO: check leaks
  // if (reqBody.empty()) {
  if (_bodyBytesSent == reqBody.size()) {
    server.getEpoll().remove(bodyPipe[1]);
    close(bodyPipe[1]);
    server.getCgiMap().erase(bodyPipe[1]);
    bodyPipe[1] = -1;
  }
}

Cgi::~Cgi() {
  if (outputPipe[0] != -1) {
    server.getEpoll().remove(outputPipe[0]);
    server.getCgiMap().erase(outputPipe[0]);
    close(outputPipe[0]);
  }
  if (bodyPipe[1] != -1) {
    server.getEpoll().remove(bodyPipe[1]);
    server.getCgiMap().erase(bodyPipe[1]);
    close(bodyPipe[1]);
  }

  int status;
  if (waitpid(childPid, &status, WNOHANG) == 0) {
    kill(childPid, SIGKILL);
    waitpid(childPid, &status, 0);
  }
}

void Cgi::handleEvent(int triggeredFd) {
  if (triggeredFd == bodyPipe[1]) {
    sendingBody();
  } else if (triggeredFd == outputPipe[0]) {
    readingOutput();
  } else {
    throw std::runtime_error("unknown cgiState");
  }
}

std::string Cgi::getOutput() { return output; }

t_state Cgi::getState() { return state; }

std::vector<std::string> Cgi::buildEnv() {
  std::vector<std::string> env;
  env.push_back("REQUEST_METHOD=" + _req.getMethod());
  // TODO: precise CONTENT_LENGTH
  env.push_back("CONTENT_LENGTH=" + toString(_req.getBody().size()));
  // env.push_back("CONTENT_LENGTH=" + _req.getHeader("content-length"));
  env.push_back("CONTENT_TYPE=" + _req.getHeader("content-type"));
  env.push_back("QUERY_STRING=" + extractQuery());
  env.push_back("SERVER_PROTOCOL=HTTP/1.1");
  env.push_back("PATH_INFO=" + extractPath());
  env.push_back("REQUEST_URI=" + _req.getUri());
  addHeaderEnvVars(env);
  return env;
}

void Cgi::addHeaderEnvVars(std::vector<std::string> &env) {
  const std::map<std::string, std::string> &headers = _req.getHeaders();

  for (std::map<std::string, std::string>::const_iterator it = headers.begin();
       it != headers.end(); ++it) {
    if (it->first == "content-length" || it->first == "content-type")
      continue;
    env.push_back(headerNameToEnvVar(it->first) + "=" + it->second);
  }
}

std::string Cgi::headerNameToEnvVar(const std::string &headerName) {
  std::string envVar = headerName;

  std::replace(envVar.begin(), envVar.end(), '-', '_');
  std::transform(envVar.begin(), envVar.end(), envVar.begin(), ::toupper);
  return "HTTP_" + envVar;
}
// std::vector<std::string> Cgi::buildEnv() {
//   size_t queryPos = _req.getUri().find('?');
//   std::string pathInfo = (queryPos != std::string::npos)
//                              ? _req.getUri().substr(0, queryPos)
//                              : _req.getUri();
//
//   std::vector<std::string> env;
//   env.push_back("REQUEST_METHOD=" + _req.getMethod());
//   env.push_back("CONTENT_LENGTH=" + _req.getHeader("content-length"));
//   env.push_back("CONTENT_TYPE=" + _req.getHeader("content-type"));
//   env.push_back("QUERY_STRING=" + extractQuery());
//   env.push_back("SERVER_PROTOCOL=HTTP/1.0");
//   env.push_back("REQUEST_URI=" + _req.getUri());
//   env.push_back("PATH_INFO=" + pathInfo);
//   return env;
// }

std::string Cgi::extractQuery() {
  size_t queryPos = _req.getUri().find('?');

  if (queryPos != std::string::npos)
    return _req.getUri().substr(queryPos + 1);
  else
    return "";
}

std::string Cgi::extractPath() {
  size_t queryPos = _req.getUri().find('?');

  if (queryPos != std::string::npos)
    return _req.getUri().substr(0, queryPos);
  else
    return _req.getUri();
}

std::string Cgi::extractExtension() {
  size_t dotPos = path.find_last_of('.');
  if (dotPos == std::string::npos) {
    // TODO: better to just return error response
    throw std::runtime_error("cgi script without extension");
  }
  return path.substr(dotPos);
}

std::string Cgi::getInterpreter(const std::string &ext) {
  if (ext == ".py")
    return "/usr/bin/python3";
  if (ext == ".php")
    return "/usr/bin/php-cgi";
  if (ext == ".sh")
    return "/bin/bash";
  if (ext == ".bla") {
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
      throw std::runtime_error("getcwd: " + std::string(strerror(errno)));
    return std::string(cwd) + "/cgi_tester";
  }
  // TODO: better to just return an error response
  return "";
}
