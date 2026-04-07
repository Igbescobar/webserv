/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: igngonza <igngonza@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 13:45:10 by igngonza          #+#    #+#             */
/*   Updated: 2026/04/07 13:45:21 by igngonza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_CONFIG_HPP
#define SERVER_CONFIG_HPP

#include "LocationConfig.hpp"
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
  const std::map<int, std::string> &getErrorPages() const;
  long getClientMaxBodySize() const;
  const std::vector<LocationConfig> &getLocations() const;
  const LocationConfig *findLocation(const std::string &path) const;

  void addListen(const std::string &listen_directive);
  void addServerName(const std::string &name);
  void addErrorPage(int code, const std::string &path);
  void setClientMaxBodySize(long size);
  void addLocation(const LocationConfig &location);

private:
  std::vector<std::string> _listen;
  std::vector<std::string> _serverNames;
  std::map<int, std::string> _errorPages;
  long _clientMaxBodySize;
  std::vector<LocationConfig> _locations;
};

#endif