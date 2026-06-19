#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP
#include "../inc/request/HttpRequest.hpp"
#include <iostream>
#include <string>

class Cgi {
private:
  HttpRequest &request;
  const LocationConfig &location;
  std::string scriptPath;
  std::string interpreter;
  std::string query;
  std::string output;
  t_state state;

  std::string getInterpreter(const std::string &ext);
  std::string extractScriptPath();
  std::string extractExtension();
  std::string extractQuery();
  std::vector<std::string> buildEnv();
  std::string readPipe(int stdoutPipe[2]);
  void setupChild(int stdinpipe[2], int stdoutpipe[2], char *argv[],
                  char **envp);
public:
  Cgi(HttpRequest &request, const LocationConfig &location);
  ~Cgi();
  std::string execute();
  std::string getOutput();
  bool handleEvent();
  t_state getState();
  std::string buildResponse(std::string &output);
};
#endif
