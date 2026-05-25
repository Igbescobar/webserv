#pragma once

#include "server/Server.hpp"
#include <string>

class Server;
class Client;
class Cgi;

class Cgi {
private:
  Server &server;
  std::string path;

public:
  Cgi(Server &server, std::string path);
};
