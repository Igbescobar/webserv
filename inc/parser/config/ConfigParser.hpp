#pragma once

#include "parser/config/LocationConfig.hpp"
#include "parser/config/ServerConfig.hpp"

class ConfigParser {
public:
  ConfigParser();
  ConfigParser(const ConfigParser &other);
  ConfigParser &operator=(const ConfigParser &other);
  ~ConfigParser();

  void parse(const std::string &configPath);
  void validateServerConfigs() const;
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
  void parseServerDirective(ServerConfig &config);
  void parseListen(ServerConfig &config);
  void parseServerName(ServerConfig &config);
  void parseRoot(ServerConfig &config);
  void parseClientMaxBodySize(ServerConfig &config);
  void parseIndex(ServerConfig &config);
  void parseErrorPage(ServerConfig &config);
  void parseLocationCgiPass(LocationConfig &location);

  std::string normalizeListen(const std::string &raw);
  std::string extractHost(const std::string &raw) const;
  std::string extractPort(const std::string &raw) const;
  std::string normalizeHost(const std::string &host) const;
  bool isFullNumber(const std::string &str) const;
  void validatePort(const std::string &portStr) const;
  void validateIP(const std::string &ip) const;
  bool isValidOctet(const std::string &octet) const;
  size_t countChar(const std::string &str, char c) const;

  void validateServerName(const ServerConfig &config,
                          const std::string &name) const;
  bool isValidSizeName(const std::string &name) const;
  bool containsValidChars(const std::string &name) const;
  bool isDuplicateServerName(const ServerConfig &config,
                             const std::string &name) const;

  long validateMaxBodySize(const std::string &rawValue) const;
  std::string getNumberPart(const std::string &rawValue) const;
  long getMultiplier(const std::string &rawValue) const;

  std::vector<int> collectErrorCodes();
  int validateAndParseErrorCode(const std::string &codeStr) const;
  std::string getErrorPagePath();

  void validateHasValue(const std::string &directive) const;
  void validateOnlyOneValue(const std::string &directive) const;
  void validateNotDuplicate(const std::string &directive, bool isSet) const;
  void skipComment(const std::string &content, size_t &index);
  void pushCurrentToken(std::string &token);
  bool isDelimiter(char c) const;
  bool isWhitespace(char c) const;
  void validateHasServerBlocks() const;
  void validateEachServerHasMandatoryDirectives() const;
  void validateNoDuplicateIpPortAcrossServers() const;
  void validateCgiPassExtension(const std::string &ext);

  void parseLocation(ServerConfig &config);
  void parseLocationDirective(LocationConfig &location);

  void parseLocationRoot(LocationConfig &location);
  void parseLocationIndex(LocationConfig &location);
  void parseLocationAutoIndex(LocationConfig &location);
  void parseLocationAllowedMethods(LocationConfig &location);
  void parseLocationReturn(LocationConfig &location);
  void parseLocationUploadPath(LocationConfig &location);
  void parseLocationClientMaxBodySize(LocationConfig &location);
};
