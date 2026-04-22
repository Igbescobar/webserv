#ifndef SERVER_CONFIG_HPP
#define SERVER_CONFIG_HPP

#include <map>
#include <string>
#include <vector>

class ServerConfig {
public:
  ServerConfig();
  ~ServerConfig();
  ServerConfig(const ServerConfig &other);
  ServerConfig &operator=(const ServerConfig &other);

  const std::vector<std::string> &getListens() const;
  const std::vector<std::string> &getIPs() const;
  const std::vector<int> &getPorts() const;
  const std::vector<std::string> &getServerNames() const;
  const std::string &getRoot() const;
  long getClientMaxBodySize() const;
  const std::vector<std::string> &getIndexFiles() const;
  const std::map<int, std::string> &getErrorPages() const;

  void addListen(const std::string &listen_directive);
  void addIP(const std::string &ip);
  void addPort(const int &port);
  void addServerName(const std::string &serverName);
  void setRoot(const std::string &root);
  void setClientMaxBodySize(const long &size);
  void addIndexFile(const std::string &file);
  void addErrorPage(int code, const std::string &path);

private:
  std::vector<std::string> listen;
  std::vector<std::string> ips;
  std::vector<int> ports;
  std::vector<std::string> serverNames;
  std::string root;
  long clientMaxBodySize;
  std::vector<std::string> indexFiles;
  std::map<int, std::string> errorPages;
};

#endif
