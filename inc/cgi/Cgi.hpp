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

typedef enum e_cgiState { SENDING_BODY, READING_OUTPUT } t_cgiState;

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
  // int bodyPipe[2];
  t_state state;
  t_cgiState cgiState;
  pid_t childPid;

  std::vector<std::string> buildEnv();
  std::string extractQuery();

public:
  Cgi(Server &server, std::string path, HttpRequest &req);

  void handleEvent();

  std::string getOutput();
  t_state getState();
};
