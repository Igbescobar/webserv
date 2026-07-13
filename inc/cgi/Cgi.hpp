#pragma once
#include "request/HttpRequest.hpp"
#include "server/Server.hpp"
#include <iostream>
#include <string>


enum e_cgi_event {
    CGI_CONTINUE,
    CGI_STDIN_DONE,
    CGI_COMPLETE,
    CGI_ERROR
};

class Server;
class Cgi {
private:
  HttpRequest &request;
  const LocationConfig &location;
  std::string scriptPath;
  std::string interpreter;
  std::string query;
  std::string output;
  t_state state;
  int clientFd;
  int pipeFd;
  pid_t pid;
  int errorCode;
  int stdinPipeFd;
  size_t bodyWritten;
  std::string body;

  std::string getInterpreter(const std::string &ext);
  std::string extractScriptPath();
  std::string extractExtension();
  std::string extractQuery();
  std::vector<std::string> buildEnv();
  std::string readPipe(int stdoutPipe[2]);
  e_cgi_event writeBody();
  e_cgi_event readOutput();
  void setupChild(int stdinpipe[2], int stdoutpipe[2], char *argv[],
                  char **envp);

public:
  Cgi(HttpRequest &request, const LocationConfig &location, int clientFd);
  ~Cgi();
  void execute(Server &server);
  std::string getOutput();
  e_cgi_event handleEvent(int fd);
  t_state getState();
  int getErrorCode();
  int getClientFd();
};
