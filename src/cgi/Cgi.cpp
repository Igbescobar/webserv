#include "../inc/parser/config/LocationConfig.hpp"
#include "cgi/Cgi.hpp"
#include <sstream>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#define BUF_SIZE 4096

Cgi::Cgi(HttpRequest &request, const LocationConfig &location, int clientFd)
    : request(request), location(location), clientFd(clientFd) {}

Cgi::~Cgi() {}

std::string Cgi::getInterpreter(const std::string &ext) {
  if (ext == ".py")
    return "/usr/bin/python3";
  if (ext == ".php")
    return "/usr/bin/php-cgi";
  if (ext == ".sh")
    return "/bin/bash";
  return "";
}

bool Cgi::handleEvent() {
 char buf[BUF_SIZE];
 int bytes = ::read(pipeFd, buf, BUF_SIZE);
 if (bytes > 0) {
   output += std::string(buf, bytes);
   return false;
 }
 if (bytes == 0) {
  waitpid(pid, NULL, WNOHANG);
  state = COMPLETE;
  return true;
 }
 state = ERROR;
 return true;
}

std::string Cgi::extractScriptPath() {
  size_t filePosition = request.getUri().find_last_of('/');
  if (filePosition == std::string::npos) {
    std::cout << "Does not have a correct file name";
    return "";
  }
  size_t queryPos = request.getUri().find('?', filePosition);
  std::string fileName =
      (queryPos != std::string::npos)
          ? request.getUri().substr(filePosition, queryPos - filePosition)
          : request.getUri().substr(filePosition);
  fileName.erase(0, 1);
  return location.getRoot() + fileName;
}

std::string Cgi::extractExtension() {
  size_t dotPos = request.getUri().find_last_of('.');
  if (dotPos == std::string::npos) {
    std::cout << "Does not have a correct file extension";
    return "";
  }
  size_t questionpos = request.getUri().find('?', dotPos);
  std::string extensionPath =
      (questionpos != std::string::npos)
          ? request.getUri().substr(dotPos, questionpos - dotPos)
          : request.getUri().substr(dotPos);
  std::cout << "Extesion path:" << extensionPath << "\n";
  return extensionPath;
}

std::string Cgi::extractQuery() {
  size_t queryPos = request.getUri().find('?');
  return (queryPos != std::string::npos) ? request.getUri().substr(queryPos + 1)
                                         : "";
}

std::vector<std::string> Cgi::buildEnv() {
  std::vector<std::string> env;
  env.push_back("REQUEST_METHOD=" + request.getMethod());
  env.push_back("CONTENT_LENGTH=" + request.getHeader("content-length"));
  env.push_back("CONTENT_TYPE=" + request.getHeader("content-type"));
  env.push_back("QUERY_STRING=" + query);
  return env;
}

void Cgi::setupChild(int stdinpipe[2], int stdoutpipe[2], char *argv[],
                            char **envp) {
  std::string scriptDir = scriptPath.substr(0, scriptPath.find_last_of('/'));
  chdir(scriptDir.c_str());
  dup2(stdinpipe[0], STDIN_FILENO);
  dup2(stdoutpipe[1], STDOUT_FILENO);
  std::cerr<<"[CGI] value of stdinpipe[0] and stdoutpipe[1] after dup2: "<<stdinpipe[0]<<" "<<stdoutpipe[1]<<"\n";
  close(stdinpipe[1]);
  close(stdoutpipe[0]);
  execve(interpreter.c_str(), argv, envp);
  exit(1);
}

void Cgi::execute(Server &server) {
  scriptPath = extractScriptPath();
  interpreter = getInterpreter(extractExtension());
  query = extractQuery();
  char *argv[] = {const_cast<char *>(interpreter.c_str()),
                  const_cast<char *>(scriptPath.c_str()), NULL};

  std::vector<std::string> env = buildEnv();

  std::vector<char *> envp;
  std::cout << "ENV: " << "\n";
  for (size_t i = 0; i < env.size(); i++) {
    std::cout << env[i] << "\n";
    envp.push_back(const_cast<char *>(env[i].c_str()));
  }
  envp.push_back(NULL);

  int stdinpipe[2];
  int stdoutpipe[2];

  if (pipe(stdinpipe) < 0 || pipe(stdoutpipe) < 0)
    return ;
  std::cerr << "[CGI] created pipes: stdin[" << stdinpipe[0] << "," << stdinpipe[1] 
              << "] stdout[" << stdoutpipe[0] << "," << stdoutpipe[1] << "]\n";
  pid = fork();
  if (pid < 0)
    return ;
  if (pid == 0)
    setupChild(stdinpipe, stdoutpipe, argv, envp.data());
  else {
    close(stdinpipe[0]);
    close(stdoutpipe[1]);
    std::string body = request.getBody();
    write(stdinpipe[1], body.c_str(), body.size());
    close(stdinpipe[1]);
    this->pipeFd = stdoutpipe[0];
    fcntl(stdoutpipe[0], F_SETFL, O_NONBLOCK);
    server.getCgiMap()[stdoutpipe[0]] = this;
    server.getEpoll().addRead(stdoutpipe[0]);
  }
  return ;
}

t_state Cgi::getState()
{
  return state;
}

std::string Cgi::getOutput()
{
  return this->output;
}

int Cgi::getClientFd()
{
	return clientFd;
}
