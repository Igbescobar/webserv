#pragma once

#include "parser/config/LocationConfig.hpp"
#include <map>

class ServerConfig {
public:
  ServerConfig();
  ServerConfig(const ServerConfig &other);
  ServerConfig &operator=(const ServerConfig &other);
  ~ServerConfig();

  const std::vector<std::string> &getListens() const;
  const std::vector<std::string> &getIPs() const;
  const std::vector<int> &getPorts() const;
  const std::vector<std::string> &getServerNames() const;
  const std::string &getRoot() const;
  long getClientMaxBodySize() const;
  const std::vector<std::string> &getIndexFiles() const;
  const std::map<int, std::string> &getErrorPages() const;
  const std::vector<LocationConfig> &getLocations() const;

  void addListen(const std::string &listen_directive);
  void addIP(const std::string &ip);
  void addPort(const int &port);
  void addServerName(const std::string &serverName);
  void setRoot(const std::string &root);
  void setClientMaxBodySize(const long &size);
  void addIndexFile(const std::string &file);
  void addErrorPage(int code, const std::string &path);
  void addLocation(const LocationConfig &location);

private:
  std::vector<std::string> listen;
  std::vector<std::string> ips;
  std::vector<int> ports;
  std::vector<std::string> serverNames;
  std::string root;
  long clientMaxBodySize;
  std::vector<std::string> indexFiles;
  std::map<int, std::string> errorPages;
  std::vector<LocationConfig> locations;
};
