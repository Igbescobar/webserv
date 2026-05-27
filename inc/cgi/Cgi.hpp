#pragma once

#include "request/HttpRequest.hpp"
#include "server/Server.hpp"
#include <string>

#define CGI_SAMPLE_OUTPUT                                                      \
  "Content-Type: text/plain\n"                                                 \
  "Content-Length: 13\n"                                                       \
  "\n"                                                                         \
  "Hello world!\n"

class Server;
class Client;
class Cgi;

// TODO: fds non-blocking! cloexec?

class Cgi {
private:
  Server &server;
  std::string path;
  std::string output;
  int pipefd[2];
  t_state state;

public:
  Cgi(Server &server, std::string path);

  bool handleEvent();

  std::string getOutput();
  t_state getState();
};
