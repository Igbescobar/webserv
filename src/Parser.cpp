/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: igngonza <igngonza@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 14:18:20 by igngonza          #+#    #+#             */
/*   Updated: 2026/04/07 17:09:17 by igngonza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Parser.hpp"
#include <iostream>

Parser::Parser() : _currentTokenIndex(0) {}

Parser::~Parser() {}

void Parser::tokenizeFile(const std::string &filename) {
  std::ifstream file(filename.c_str());
  if (!file.is_open()) {
    throw std::runtime_error("Error: Could not open configuration file: " +
                             filename);
  }

  std::stringstream contentStream;
  contentStream << file.rdbuf();
  std::string content = contentStream.str();
  file.close();

  std::string currentToken;
  for (size_t i = 0; i < content.length(); ++i) {
    char c = content[i];
    if (c == '#') {
      while (i < content.length() && content[i] != '\n')
        i++;
      continue;
    }
    if (c == '{' || c == '}' || c == ';') {
      if (!currentToken.empty())
        _tokens.push_back(currentToken);
      currentToken.clear();
      _tokens.push_back(std::string(1, c));
      continue;
    }
    if (isspace(c)) {
      if (!currentToken.empty())
        _tokens.push_back(currentToken);
      currentToken.clear();
    } else {
      currentToken += c;
    }
  }
  if (!currentToken.empty())
    _tokens.push_back(currentToken);
}

std::vector<ServerConfig> Parser::parse(const std::string &filename) {
  tokenizeFile(filename);
  std::vector<ServerConfig> servers;

  while (hasMoreTokens()) {
    if (peekNextToken() == "server") {
      getNextToken();
      parseServerBlock(servers);
    } else {
      throw std::runtime_error("Error: Expected 'server' keyword.");
    }
  }
  return servers;
}

void Parser::parseServerBlock(std::vector<ServerConfig> &servers) {
  ServerConfig currentServer;
  LocationConfig serverDefaults;

  if (getNextToken() != "{") {
    throw std::runtime_error("Error: Expected '{' after 'server' keyword.");
  }

  while (hasMoreTokens() && peekNextToken() != "}") {
    const std::string &directive = getNextToken();

    if (directive == "listen") {
      currentServer.addListen(getNextToken());
      if (getNextToken() != ";")
        throw std::runtime_error("Error: Missing ';' after listen.");
    } else if (directive == "server_name") {
      currentServer.addServerName(getNextToken());
      if (getNextToken() != ";")
        throw std::runtime_error("Error: Missing ';' after server_name.");
    } else if (directive == "root") {
      serverDefaults.setRoot(getNextToken());
      if (getNextToken() != ";")
        throw std::runtime_error("Error: Missing ';' after root.");
    } else if (directive == "autoindex") {
      const std::string &value = getNextToken();
      if (value == "on") {
        serverDefaults.setAutoIndex(true);
      } else if (value == "off") {
        serverDefaults.setAutoIndex(false);
      } else {
        throw std::runtime_error(
            "Error: autoindex value must be 'on' or 'off'.");
      }
      if (getNextToken() != ";")
        throw std::runtime_error("Error: Missing ';' after autoindex.");
    } else if (directive == "index") {
      while (peekNextToken() != ";") {
        serverDefaults.addIndexFile(getNextToken());
      }
      getNextToken(); // consume ;
    } else if (directive == "location") {
      // Pass the server defaults down to the location block.
      parseLocationBlock(currentServer, serverDefaults);
    } else {
      throw std::runtime_error("Error: Unknown directive in server block: " +
                               directive);
    }
  }

  if (getNextToken() != "}") {
    throw std::runtime_error("Error: Expected '}' to close server block.");
  }

  servers.push_back(currentServer);
}

void Parser::parseLocationBlock(ServerConfig &server,
                                const LocationConfig &defaults) {
  LocationConfig currentLocation = defaults;

  currentLocation.setPath(getNextToken());

  if (getNextToken() != "{") {
    throw std::runtime_error("Error: Expected '{' after location path.");
  }

  while (hasMoreTokens() && peekNextToken() != "}") {
    const std::string &directive = getNextToken();
    if (directive == "index") {
      currentLocation.getIndexFiles().clear();
      while (peekNextToken() != ";") {
        currentLocation.addIndexFile(getNextToken());
      }
      getNextToken();
    } else if (directive == "allow_methods") {
      currentLocation.getAllowedMethods().clear();
      while (peekNextToken() != ";") {
        currentLocation.addAllowedMethod(getNextToken());
      }
      getNextToken();
    } else if (directive == "root") {
      currentLocation.setRoot(getNextToken());
      if (getNextToken() != ";")
        throw std::runtime_error("Error: Missing ';' after root.");
    } else if (directive == "autoindex") {
      const std::string &value = getNextToken();
      if (value == "on")
        currentLocation.setAutoIndex(true);
      else if (value == "off")
        currentLocation.setAutoIndex(false);
      else
        throw std::runtime_error(
            "Error: autoindex value must be 'on' or 'off'.");
      if (getNextToken() != ";")
        throw std::runtime_error("Error: Missing ';' after autoindex.");
    } else {
      throw std::runtime_error("Error: Unknown directive in location block: " +
                               directive);
    }
  }

  if (getNextToken() != "}") {
    throw std::runtime_error("Error: Expected '}' to close location block.");
  }
  server.addLocation(currentLocation);
}

bool Parser::hasMoreTokens() const {
  return _currentTokenIndex < _tokens.size();
}

const std::string &Parser::getNextToken() {
  if (!hasMoreTokens())
    throw std::runtime_error("Error: Unexpected end of file.");
  return _tokens[_currentTokenIndex++];
}

const std::string &Parser::peekNextToken() const {
  if (!hasMoreTokens())
    throw std::runtime_error("Error: Unexpected end of file.");
  return _tokens[_currentTokenIndex];
}
