#pragma once

#include "parser_config/ServerConfig.hpp"
#include "request/HttpRequest.hpp"
#include "response/HttpResponse.hpp"
#include "server/Epoll.hpp"
#include "server/Server.hpp"

#define BUF_SIZE 4096
#define IDLE_LIMIT 3000
#define ABSOLUTE_LIMIT 18000
#define REQUEST_LIMIT 200000000

class Server;
class Cgi;

class Client {
private:
  int clientFd;
  const ServerConfig &serverConfig;
  HttpRequest request;
  int requestSize;
  std::string responseStr;
  time_t connectionStart;
  time_t lastActivity;
  Server &server;
  Cgi *cgi;
  std::string::size_type bytesSent;
  int reference;

  bool read(int clientFd);
  bool write(int clientFd);

  void handleRequestState(t_state state);

  void updateActivity();

public:
  Client(int clientSocket, Server &, const ServerConfig &);
  ~Client();

  bool handleEvent(uint32_t eventsMask);
  bool isTimedOut();
};
