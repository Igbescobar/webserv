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

  const std::vector<std::string> &getListen() const;
  const std::vector<std::string> &getServerNames() const;
  const std::string &getRoot() const;
  long getClientMaxBodySize() const;
  const std::vector<std::string> &getIndexFiles() const;

  void setListen(const std::string &listen_directive);
  void setServerName(const std::string &serverName);
  void setRoot(const std::string &root);
  void setClientMaxBodySize(const long &size);
  void setIndexFile(const std::string &file);

private:
  std::vector<std::string> listen;
  std::vector<std::string> serverNames;
  std::string root;
  long clientMaxBodySize;
  std::vector<std::string> indexFiles;
};

#endif
