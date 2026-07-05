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

class Cgi {
private:
  Server &server;
  std::string path;
  std::string output;
  HttpRequest req;
  int outputPipe[2];
  int bodyPipe[2];
  t_state state;

  std::vector<std::string> buildEnv();
  std::string extractQuery();

public:
  Cgi(Server &server, std::string path, HttpRequest &req);

  void handleEvent();

  std::string getOutput();
  t_state getState();
};
