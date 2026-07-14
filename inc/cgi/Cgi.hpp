#pragma once

#include "request/HttpRequest.hpp"
#include "server/Server.hpp"
#include <string>

#define CGI_SAMPLE_OUTPUT                                                      \
  "Content-Type: text/plain\n"                                                 \
  "Content-Length: 18\n"                                                       \
  "\n"                                                                         \
  "CGI_SAMPLE_OUTPUT\n"

class Server;
class Client;
class Cgi;

// TODO: fds non-blocking! cloexec?
//
// TODO:
// kill process if it takes too long
// use /usr/bin/python3, php, cgi-tester, etc
// send body to cgi
class Cgi {
private:
  Server &server;
  std::string path;
  std::string output;
  HttpRequest _req;
  int outputPipe[2];
  int bodyPipe[2];
  t_state state;
  pid_t childPid;
  std::string reqBody;
  std::string interpreter;
  std::string::size_type _bodyBytesSent;

  std::vector<std::string> buildEnv();
  void addHeaderEnvVars(std::vector<std::string> &env);
  std::string headerNameToEnvVar(const std::string &headerName);
  std::string extractQuery();
  std::string extractPath();
  void sendingBody();
  void readingOutput();
  std::string extractExtension();
  std::string getInterpreter(const std::string &ext);

  Cgi(const Cgi &other);
  Cgi &operator=(const Cgi &other);

public:
  Cgi(Server &server, std::string path, HttpRequest &req);
  ~Cgi();

  void handleEvent(int triggeredFd);

  std::string getOutput();
  t_state getState();
};
