#pragma once

#include "parser/config/ServerConfig.hpp"
#include "request/HttpRequest.hpp"
#include "response/HttpResponse.hpp"
#include "server/Epoll.hpp"
#include "server/Server.hpp"

#define BUF_SIZE 4096
#define IDLE_LIMIT 30
#define ABSOLUTE_LIMIT 180
#define REQUEST_LIMIT 10000000

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

  bool read(int clientFd);
  bool write(int clientFd);

  bool isCGI();
  const LocationConfig *getMatchingLocation(const std::string &uri);

  void handleRequestState(t_state state);

  void updateActivity();

public:
  Client(int clientSocket, Server &, const ServerConfig &);
  ~Client();

  bool handleEvent(uint32_t eventsMask);
  bool isTimedOut();
};
