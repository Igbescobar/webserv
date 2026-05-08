#include "parser_config/ServerConfig.hpp"

ServerConfig::ServerConfig() : clientMaxBodySize(-1) {}

ServerConfig::ServerConfig(const ServerConfig &other)
    : listen(other.listen), ips(other.ips), ports(other.ports),
      serverNames(other.serverNames), root(other.root),
      clientMaxBodySize(other.clientMaxBodySize), indexFiles(other.indexFiles),
      errorPages(other.errorPages), locations(other.locations) {}

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
    this->locations = other.locations;
  }
  return *this;
}

ServerConfig::~ServerConfig() {}

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

const std::vector<LocationConfig> &ServerConfig::getLocations() const {
  return locations;
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

void ServerConfig::addLocation(const LocationConfig &location) {
  locations.push_back(location);
}

const LocationConfig *
ServerConfig::resolveLocation(const std::string &uri) const {
  const LocationConfig *bestMatch = NULL;
  size_t longestMatchLength = 0;

  for (size_t i = 0; i < this->locations.size(); i++) {
    const std::string &pattern = locations[i].getPattern();

    if (uri.find(pattern) == 0) {
      if (pattern.length() > longestMatchLength) {
        longestMatchLength = pattern.length();
        bestMatch = &locations[i];
      }
    }
  }
  return bestMatch;
}