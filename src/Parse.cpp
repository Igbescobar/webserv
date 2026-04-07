/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parse.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: igngonza <igngonza@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 14:18:20 by igngonza          #+#    #+#             */
/*   Updated: 2026/04/07 14:20:29 by igngonza         ###   ########.fr       */
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
      while (i < content.length() && content[i] != '\n') {
        i++;
      }
      continue;
    }

    if (c == '{' || c == '}' || c == ';') {
      if (!currentToken.empty()) {
        _tokens.push_back(currentToken);
        currentToken.clear();
      }
      _tokens.push_back(std::string(1, c));
      continue;
    }

    if (isspace(c)) {
      if (!currentToken.empty()) {
        _tokens.push_back(currentToken);
        currentToken.clear();
      }
    } else {
      currentToken += c;
    }
  }
  if (!currentToken.empty()) {
    _tokens.push_back(currentToken);
  }
}

std::vector<ServerConfig> Parser::parse(const std::string &filename) {
  tokenizeFile(filename);

  std::vector<ServerConfig> servers;

  while (hasMoreTokens()) {
    const std::string &token = getNextToken();
    if (token == "server") {
      parseServerBlock(servers);
    } else {
      throw std::runtime_error(
          "Error: Unexpected token outside of server block: " + token);
    }
  }

  if (servers.empty()) {
    throw std::runtime_error(
        "Error: No server blocks found in configuration file.");
  }

  return servers;
}

void Parser::parseServerBlock(std::vector<ServerConfig> &servers) {

  std::cout << "Found a server block. Parsing..." << std::endl;

  if (getNextToken() != "{") {
    throw std::runtime_error("Error: Expected '{' after 'server' keyword.");
  }

  while (hasMoreTokens() && peekNextToken() != "}") {
    getNextToken();
  }

  if (getNextToken() != "}") {
    throw std::runtime_error("Error: Expected '}' to close server block.");
  }
  std::cout << "Finished parsing server block." << std::endl;
}

bool Parser::hasMoreTokens() const {
  return _currentTokenIndex < _tokens.size();
}

const std::string &Parser::getNextToken() {
  if (!hasMoreTokens()) {
    throw std::runtime_error(
        "Error: Unexpected end of file. Reached end of tokens.");
  }
  return _tokens[_currentTokenIndex++];
}

const std::string &Parser::peekNextToken() const {
  if (!hasMoreTokens()) {
    throw std::runtime_error(
        "Error: Unexpected end of file. Reached end of tokens.");
  }
  return _tokens[_currentTokenIndex];
}

void Parser::parseLocationBlock(LocationConfig &location,
                                ServerConfig &server) {
  (void)location;
  (void)server;
}