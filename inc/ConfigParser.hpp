#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "../inc/ServerConfig.hpp"
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

class ConfigParser {
public:
  ConfigParser();
  ~ConfigParser();

  void parse(const std::string &configPath);
  const std::vector<ServerConfig> &getServerConfigs() const;

private:
  std::vector<std::string> tokens;
  size_t tokenPosition;
  std::vector<ServerConfig> serverConfigs;

  std::string readFileContents(const std::string &filename);
  void generateTokens(const std::string &content);
  void tokenizeFile(const std::string &filename);
  bool hasMoreTokens() const;
  const std::string &peekToken() const;
  void consumeToken(const std::string &expected);
  void validateSemicolon();
  const std::vector<std::string> &getTokens() const;

  void parseServerBlock();
  void parseListen(ServerConfig &config);
  void parseServerDirective(ServerConfig &config);
  std::string normalizeListen(const std::string &raw);
  bool isFullNumber(const std::string &str) const;
  void validatePort(const std::string &portStr) const;
  void validateIP(const std::string &ip) const;
  bool isValidOctet(const std::string &octet) const;
  size_t countChar(const std::string &str, char c) const;

  void skipComment(const std::string &content, size_t &index);
  void pushCurrentToken(std::string &token);
  bool isDelimiter(char c) const;
  bool isWhitespace(char c) const;
};

#endif
