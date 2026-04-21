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

  void addListen(const std::string &listen_directive);
  void addServerName(const std::string &serverName);
  void addRoot(const std::string &root);

private:
  std::vector<std::string> listen;
  std::vector<std::string> serverNames;
  std::string root;
};

#endif
