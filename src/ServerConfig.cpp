#include "../inc/ServerConfig.hpp"
#include <cstddef>

ServerConfig::ServerConfig() {}

ServerConfig::~ServerConfig() {}

ServerConfig::ServerConfig(const ServerConfig &other) { *this = other; }

ServerConfig &ServerConfig::operator=(const ServerConfig &other) {
  if (this != &other) {
    this->listen = other.listen;
    this->serverNames = other.serverNames;
  }
  return *this;
}

const std::vector<std::string> &ServerConfig::getListen() const {
  return this->listen;
}

const std::vector<std::string> &ServerConfig::getServerNames() const {
  return this->serverNames;
};

void ServerConfig::addListen(const std::string &listen_directive) {
  this->listen.push_back(listen_directive);
}

void ServerConfig::addServerName(const std::string &serverName) {
  this->serverNames.push_back(serverName);
}
