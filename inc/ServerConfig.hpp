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

  void addListen(const std::string &listen_directive);

private:
  std::vector<std::string> listen;
};

#endif
