#include "../../../inc/parser/config/ConfigParser.hpp"
#include "../../../inc/parser/config/ServerConfig.hpp"
#include <cstdlib>
#include <stdexcept>

ConfigParser::ConfigParser() : tokenPosition(0) {}

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
  if (this->serverConfigs.empty()) {
    throw std::runtime_error(
        "Configuration file must contain at least one server block.");
  }

  for (size_t i = 0; i < this->serverConfigs.size(); ++i) {
    const ServerConfig &config = this->serverConfigs[i];

    if (config.getListens().empty()) {
      std::stringstream errorMsg;
      errorMsg << "Server " << i
               << " is missing a required 'listen' directive.";
      throw std::runtime_error(errorMsg.str());
    }

    if (config.getRoot().empty()) {
      std::stringstream errorMsg;
      errorMsg << "Server " << i << " is missing a required 'root' directive.";
      throw std::runtime_error(errorMsg.str());
    }

    // check different servers with same ip and port
  }
}
