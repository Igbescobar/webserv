#include "parser/config/ConfigParser.hpp"
#include "parser/config/ServerConfig.hpp"
#include <cstdlib>
#include <set>
#include <sstream>
#include <stdexcept>

ConfigParser::ConfigParser() : tokenPosition(0) {}

ConfigParser::ConfigParser(const ConfigParser &other)
    : tokens(other.tokens), tokenPosition(other.tokenPosition),
      serverConfigs(other.serverConfigs) {}

ConfigParser &ConfigParser::operator=(const ConfigParser &other) {
  if (this != &other) {
    tokens = other.tokens;
    tokenPosition = other.tokenPosition;
    serverConfigs = other.serverConfigs;
  }
  return *this;
}

ConfigParser::~ConfigParser() {}

const std::vector<ServerConfig> &ConfigParser::getServerConfigs() const {
  return serverConfigs;
}

void ConfigParser::parse(const std::string &configPath) {
  tokenizeFile(configPath);

  while (hasMoreTokens()) {
    if (peekToken() == "server") {
      parseServerBlock();
    } else {
      throw std::runtime_error("Unknown setting keyword: " + peekToken());
    }
  }
}

void ConfigParser::parseServerBlock() {
  consumeToken("server");
  consumeToken("{");

  ServerConfig newServer;

  while (hasMoreTokens() && peekToken() != "}") {
    parseServerDirective(newServer);
  }

  consumeToken("}");
  serverConfigs.push_back(newServer);
}

void ConfigParser::parseServerDirective(ServerConfig &config) {
  const std::string &directive = peekToken();

  if (directive == "listen") {
    parseListen(config);
  } else if (directive == "server_name") {
    parseServerName(config);
  } else if (directive == "root") {
    parseRoot(config);
  } else if (directive == "client_max_body_size") {
    parseClientMaxBodySize(config);
  } else if (directive == "index") {
    parseIndex(config);
  } else if (directive == "error_page") {
    parseErrorPage(config);
  } else {
    throw std::runtime_error("Invalid server directive: " + directive);
  }
}

void ConfigParser::validateServerConfigs() const {
  validateHasServerBlocks();
  validateEachServerHasMandatoryDirectives();
  validateNoDuplicateIpPortAcrossServers();
}