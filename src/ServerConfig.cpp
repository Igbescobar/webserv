#include "../inc/ServerConfig.hpp"
#include <cstddef>

ServerConfig::ServerConfig() : clientMaxBodySize(-1) {}

ServerConfig::~ServerConfig() {}

ServerConfig::ServerConfig(const ServerConfig &other) { *this = other; }

ServerConfig &ServerConfig::operator=(const ServerConfig &other) {
  if (this != &other) {
    this->listen = other.listen;
    this->serverNames = other.serverNames;
    this->root = other.root;
    this->clientMaxBodySize = other.clientMaxBodySize;
    this->indexFiles = other.indexFiles;
  }
  return *this;
}

const std::vector<std::string> &ServerConfig::getListen() const {
  return this->listen;
}

const std::vector<std::string> &ServerConfig::getServerNames() const {
  return this->serverNames;
};

const std::string &ServerConfig::getRoot() const { return this->root; }

long ServerConfig::getClientMaxBodySize() const {
  return this->clientMaxBodySize;
}

const std::vector<std::string> &ServerConfig::getIndexFiles() const {
  return this->indexFiles;
}

void ServerConfig::setListen(const std::string &listen_directive) {
  this->listen.push_back(listen_directive);
}

void ServerConfig::setServerName(const std::string &serverName) {
  this->serverNames.push_back(serverName);
}

void ServerConfig::setRoot(const std::string &root) { this->root = root; }

void ServerConfig::setClientMaxBodySize(const long &size) {
  this->clientMaxBodySize = size;
}

void ServerConfig::setIndexFile(const std::string &file) {
  this->indexFiles.push_back(file);
}
