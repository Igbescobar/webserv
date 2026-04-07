/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: igngonza <igngonza@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 13:57:08 by igngonza          #+#    #+#             */
/*   Updated: 2026/04/07 14:10:38 by igngonza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ServerConfig.hpp"
#include <cstddef>

ServerConfig::ServerConfig() : _clientMaxBodySize(-1) {}

ServerConfig::~ServerConfig() {}

ServerConfig::ServerConfig(const ServerConfig &other) { *this = other; }

ServerConfig &ServerConfig::operator=(const ServerConfig &other) {
  if (this != &other) {
    _listen = other._listen;
    _serverNames = other._serverNames;
    _errorPages = other._errorPages;
    _clientMaxBodySize = other._clientMaxBodySize;
    _locations = other._locations;
  }
  return *this;
}

const std::vector<std::string> &ServerConfig::getListen() const {
  return _listen;
}
const std::vector<std::string> &ServerConfig::getServerNames() const {
  return _serverNames;
}
const std::map<int, std::string> &ServerConfig::getErrorPages() const {
  return _errorPages;
}
long ServerConfig::getClientMaxBodySize() const { return _clientMaxBodySize; }
const std::vector<LocationConfig> &ServerConfig::getLocations() const {
  return _locations;
}

void ServerConfig::addListen(const std::string &listen_directive) {
  _listen.push_back(listen_directive);
}
void ServerConfig::addServerName(const std::string &name) {
  _serverNames.push_back(name);
}
void ServerConfig::addErrorPage(int code, const std::string &path) {
  _errorPages[code] = path;
}
void ServerConfig::setClientMaxBodySize(long size) {
  _clientMaxBodySize = size;
}
void ServerConfig::addLocation(const LocationConfig &location) {
  _locations.push_back(location);
}

const LocationConfig *
ServerConfig::findLocation(const std::string &path) const {
  const LocationConfig *bestMatch = NULL;
  size_t bestMatchLength = 0;

  for (size_t i = 0; i < _locations.size(); ++i) {
    const std::string &locPath = _locations[i].getPath();

    if (path.rfind(locPath, 0) == 0) {
      if (locPath[locPath.length() - 1] == '/' || locPath == path) {
        if (locPath.length() > bestMatchLength) {
          bestMatch = &_locations[i];
          bestMatchLength = locPath.length();
        }
      }
    }
  }
  return bestMatch;
}