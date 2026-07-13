#include "cgi/Cgi.hpp"
#include "parser_config/LocationConfig.hpp"
#include "utils.hpp"
#include <fcntl.h>
#include <sstream>
#include <sys/wait.h>
#include <unistd.h>
#define BUF_SIZE 4096

Cgi::Cgi(HttpRequest &request, const LocationConfig &location, int clientFd)
    : request(request), location(location), state(INCOMPLETE),
      clientFd(clientFd), errorCode(0) {}

Cgi::~Cgi() {}

std::string Cgi::getInterpreter(const std::string &ext) {
  if (ext == ".py")
    return "/usr/bin/python3";
  if (ext == ".php")
    return "/usr/bin/php-cgi";
  if (ext == ".sh")
    return "/bin/bash";
  if(ext == ".bla")
    return "./cgi_tester";
  return "";
}

e_cgi_event  Cgi::handleEvent(int fd) {
    std::cout<<"Entrada en handleEvent\n";
    if (fd == stdinPipeFd)
        return writeBody();
    std::cout<<"Lectuta handleEvent: \n";
    return readOutput();
}

e_cgi_event Cgi::writeBody() {
    size_t remaining = body.size() - bodyWritten;
    if (remaining == 0) {
        stdinPipeFd = -1;
        return CGI_STDIN_DONE;
    }
    ssize_t written = ::write(stdinPipeFd, 
                               body.c_str() + bodyWritten, 
                               std::min(remaining, (size_t)BUF_SIZE));
    std::cerr << "[STDIN WRITE] body size=" << body.size()
          << " written=" << written;
    if (written > 0)
        bodyWritten += written;
    if (bodyWritten >= body.size()) {
        stdinPipeFd = -1;
	return CGI_STDIN_DONE;
    }
    return CGI_CONTINUE;
}

e_cgi_event Cgi::readOutput() {
    char buf[BUF_SIZE];
    int bytes = ::read(pipeFd, buf, BUF_SIZE);
    if (bytes > 0) {
	std::cout<<"acumulacion de output en cgi\n";
        output += std::string(buf, bytes);
        return CGI_CONTINUE;
    }
    if (bytes == 0) {
        waitpid(pid, NULL, WNOHANG);
        state = COMPLETE;
        return CGI_COMPLETE;
    }
    state = ERROR;
    return CGI_ERROR;
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
  dup2(stdinpipe[0], STDIN_FILENO);
  dup2(stdoutpipe[1], STDOUT_FILENO);
  close(stdinpipe[1]);
  close(stdinpipe[0]);
  close(stdoutpipe[0]);
  close(stdoutpipe[1]);
  execve(argv[0], argv, envp);
  exit(1);
}

int Cgi::getErrorCode() { return errorCode; }

void Cgi::execute(Server &server) {
  scriptPath = extractScriptPath();
  interpreter = getInterpreter(extractExtension());
  query = extractQuery();
  std::cout<<"Script path: "<<scriptPath<<"\n";
  if (access(scriptPath.c_str(), F_OK) != 0) {
    state = ERROR;
    errorCode = 404;
    return;
  }
  if (access(scriptPath.c_str(), R_OK) != 0) {
    state = ERROR;
    errorCode = 403;
    return;
  }

  std::cout<<"Interpreter"<<interpreter<<"\n";
    std::vector<char *> argv;
  if (interpreter == "./cgi_tester") {
    argv.push_back(const_cast<char *>(interpreter.c_str()));
  } else {
    argv.push_back(const_cast<char *>(interpreter.c_str()));
    argv.push_back(const_cast<char *>(scriptPath.c_str()));
  }
  argv.push_back(NULL);

  std::vector<std::string> env = buildEnv();

  std::vector<char *> envp;
  for (size_t i = 0; i < env.size(); i++) {
    envp.push_back(const_cast<char *>(env[i].c_str()));
  }
  envp.push_back(NULL);

  int stdinpipe[2];
  int stdoutpipe[2];

  if (pipe(stdinpipe) < 0 || pipe(stdoutpipe) < 0)
    return;
  setCloseOnExec(stdinpipe[0]);
  setCloseOnExec(stdinpipe[1]);
  setCloseOnExec(stdoutpipe[0]);
  setCloseOnExec(stdoutpipe[1]);
  pid = fork();
  if (pid < 0)
    return;
  if (pid == 0)
    setupChild(stdinpipe, stdoutpipe, argv.data(), envp.data());
  else {
    close(stdinpipe[0]);
    close(stdoutpipe[1]);
    this->body = request.getBody();
    this->bodyWritten = 0;
    this->stdinPipeFd = stdinpipe[1];
    this->pipeFd = stdoutpipe[0];
    setNonBlocking(stdinpipe[1]);
    setNonBlocking(stdoutpipe[0]);
    server.getCgiMap()[stdinpipe[1]] = this;
    server.getCgiMap()[stdoutpipe[0]] = this;
    std::cout<<"-----cgiMap--------\n";
    server.printCgiMap();
    std::cout<<"-------------------\n";
    std::cout<<"fd que va a entrar a epoll: "<<stdinpipe[1]<<"\n";
    std::cout<<"fd que va a entrar a epoll: "<<stdoutpipe[0]<<"\n";
    server.getEpoll().printRegistered();
    server.getEpoll().addRead(stdoutpipe[0]);
    server.getEpoll().addWrite(stdinpipe[1]);
  }
  return;
}

t_state Cgi::getState() { return state; }

std::string Cgi::getOutput() { return this->output; }

int Cgi::getClientFd() { return clientFd; }
