#include "../../../inc/parser/config/ConfigParser.hpp"
#include "../../../inc/parser/config/ServerConfig.hpp"

void ConfigParser::parseServerName(ServerConfig &config) {
  consumeToken("server_name");
  validateHasValue("server_name");
  while (hasMoreTokens() && !isDelimiter(peekToken()[0])) {
    std::string name = peekToken();

    validateServerName(config, name);

    config.addServerName(name);
    this->tokenPosition++;
  }

  validateSemicolon();
}

void ConfigParser::validateServerName(const ServerConfig &config,
                                      const std::string &name) const {
  if (!isValidSizeName(name)) {
    throw std::runtime_error("Invalid server_name: '" + name +
                             "' has an invalid size.");
  }
  if (!containsValidChars(name)) {
    throw std::runtime_error("Invalid server_name: '" + name +
                             "' contains invalid characters.");
  }
  if (isDuplicateServerName(config, name)) {
    throw std::runtime_error("Duplicate server_name: '" + name +
                             "' is already defined for this server.");
  }
}

bool ConfigParser::isValidSizeName(const std::string &name) const {
  return (!name.empty() && name.length() <= 255);
}

bool ConfigParser::containsValidChars(const std::string &name) const {
  for (size_t i = 0; i < name.length(); ++i) {
    char c = name[i];
    if (!(std::isalnum(c) || c == '-' || c == '.' || c == '_'))
      return false;
  }
  return true;
}

bool ConfigParser::isDuplicateServerName(const ServerConfig &config,
                                         const std::string &name) const {
  const std::vector<std::string> &names = config.getServerNames();
  for (size_t i = 0; i < names.size(); ++i) {
    if (names[i] == name)
      return true;
  }
  return false;
}