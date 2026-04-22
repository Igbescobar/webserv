#include "../../../inc/parser/config/ServerConfig.hpp"

ServerConfig::ServerConfig() : clientMaxBodySize(-1) {}

ServerConfig::~ServerConfig() {}

ServerConfig::ServerConfig(const ServerConfig &other) { *this = other; }

ServerConfig &ServerConfig::operator=(const ServerConfig &other) {
  if (this != &other) {
    this->listen = other.listen;
    this->ips = other.ips;
    this->ports = other.ports;
    this->serverNames = other.serverNames;
    this->root = other.root;
    this->clientMaxBodySize = other.clientMaxBodySize;
    this->indexFiles = other.indexFiles;
    this->errorPages = other.errorPages;
  }
  return *this;
}

const std::vector<std::string> &ServerConfig::getListens() const {
  return this->listen;
}

const std::vector<std::string> &ServerConfig::getIPs() const {
  return this->ips;
}

const std::vector<int> &ServerConfig::getPorts() const { return this->ports; }

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

const std::map<int, std::string> &ServerConfig::getErrorPages() const {
  return this->errorPages;
}

void ServerConfig::addListen(const std::string &listen_directive) {
  this->listen.push_back(listen_directive);
}

void ServerConfig::addServerName(const std::string &serverName) {
  this->serverNames.push_back(serverName);
}

void ServerConfig::addIP(const std::string &ip) { this->ips.push_back(ip); }

void ServerConfig::addPort(const int &port) { this->ports.push_back(port); }

void ServerConfig::addIndexFile(const std::string &file) {
  this->indexFiles.push_back(file);
}

void ServerConfig::setRoot(const std::string &root) { this->root = root; }

void ServerConfig::setClientMaxBodySize(const long &size) {
  this->clientMaxBodySize = size;
}

void ServerConfig::addErrorPage(int code, const std::string &path) {
  this->errorPages[code] = path;
}
