#include "../inc/ConfigParser.hpp"
#include "../inc/ServerConfig.hpp"

ConfigParser::ConfigParser() : tokenPosition(0) {}

ConfigParser::~ConfigParser() {}

const std::vector<ServerConfig> &ConfigParser::getServerConfigs() const {
  return serverConfigs;
}

void ConfigParser::parse(const std::string &configPath) {
  tokenizeFile(configPath);
  this->tokenPosition = 0;

  while (hasMoreTokens()) {
    if (peekToken() == "server") {
      parseServerBlock();
    } else {
      throw std::runtime_error("Unknown setting keyword: " + peekToken());
    }
  }
}

void ConfigParser::tokenizeFile(const std::string &filename) {
  std::string rawContent = readFileContents(filename);

  generateTokens(rawContent);
}

std::string ConfigParser::readFileContents(const std::string &filename) {
  std::ifstream file(filename.c_str());
  if (!file.is_open()) {
    throw std::runtime_error("Error: Could not open config file: " + filename);
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

void ConfigParser::generateTokens(const std::string &content) {
  std::string currentToken;

  for (size_t i = 0; i < content.length(); i++) {
    char c = content[i];

    if (c == '#') {
      skipComment(content, i);
    } else if (isDelimiter(c)) {
      pushCurrentToken(currentToken);
      this->tokens.push_back(std::string(1, c));
    } else if (isWhitespace(c))
      pushCurrentToken(currentToken);
    else
      currentToken += c;
  }
}

bool ConfigParser::hasMoreTokens() const {
  return this->tokenPosition < this->tokens.size();
}

const std::string &ConfigParser::peekToken() const {
  if (!this->hasMoreTokens()) {
    throw std::runtime_error("Unexpected end of configuration file.");
  }
  return this->tokens[this->tokenPosition];
}

void ConfigParser::consumeToken(const std::string &expected) {
  if (this->peekToken() != expected) {
    throw std::runtime_error("Expected '" + expected + "' but found '" +
                             this->peekToken() + "'");
  }
  this->tokenPosition++;
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
  } else {
    throw std::runtime_error("Invalid server directive: " + directive);
  }
}

void ConfigParser::parseListen(ServerConfig &config) {
  consumeToken("listen");

  validateHasValue("listen");

  std::string rawValue = peekToken();
  this->tokenPosition++;

  std::string normalizedValue = normalizeListen(rawValue);

  config.setListen(normalizedValue);

  validateSemicolon();
}

std::string ConfigParser::normalizeListen(const std::string &raw) {
  std::string host = extractHost(raw);
  std::string port = extractPort(raw);

  host = normalizeHost(host);
  validateIP(host);
  validatePort(port);

  return host + ":" + port;
}

std::string ConfigParser::extractHost(const std::string &raw) const {
  size_t colonPos = raw.find(':');
  if (colonPos != std::string::npos) {
    return raw.substr(0, colonPos);
  } else if (isFullNumber(raw)) {
    return "0.0.0.0";
  } else {
    return raw;
  }
}

std::string ConfigParser::extractPort(const std::string &raw) const {
  size_t colonPos = raw.find(':');
  if (colonPos != std::string::npos) {
    return raw.substr(colonPos + 1);
  } else if (isFullNumber(raw)) {
    return raw;
  } else {
    return "8080";
  }
}

std::string ConfigParser::normalizeHost(const std::string &host) const {
  if (host == "localhost") {
    return "127.0.0.1";
  }
  return host;
}

bool ConfigParser::isFullNumber(const std::string &str) const {
  for (size_t i = 0; i < str.length(); ++i) {
    if (!std::isdigit(static_cast<unsigned char>(str[i])))
      return false;
  }
  return !str.empty();
}

void ConfigParser::validatePort(const std::string &portStr) const {
  if (!isFullNumber(portStr)) {
    throw std::runtime_error("Invalid port: " + portStr);
  }
  int port = std::atoi(portStr.c_str());
  if (port < 1 || port > 65535) {
    throw std::runtime_error("Port out of range (1-65535): " + portStr);
  }
}

void ConfigParser::validateIP(const std::string &ip) const {
  std::vector<std::string> parts;
  std::stringstream ss(ip);
  std::string segment;

  while (std::getline(ss, segment, '.')) {
    parts.push_back(segment);
  }

  if (parts.size() != 4 || this->countChar(ip, '.') != 3) {
    throw std::runtime_error("Invalid IP format: " + ip);
  }

  for (size_t i = 0; i < parts.size(); ++i) {
    if (!this->isValidOctet(parts[i])) {
      throw std::runtime_error("Invalid IP octet: " + parts[i] + " in " + ip);
    }
  }
}

bool ConfigParser::isValidOctet(const std::string &octet) const {
  if (octet.empty() || octet.length() > 3) {
    return false;
  }
  if (!isFullNumber(octet)) {
    return false;
  }

  int val = std::atoi(octet.c_str());
  return (val >= 0 && val <= 255);
}

size_t ConfigParser::countChar(const std::string &str, char c) const {
  size_t count = 0;
  for (size_t i = 0; i < str.length(); ++i) {
    if (str[i] == c) {
      count++;
    }
  }
  return count;
}

void ConfigParser::parseServerName(ServerConfig &config) {
  consumeToken("server_name");
  validateHasValue("server_name");
  while (hasMoreTokens() && !isDelimiter(peekToken()[0])) {
    std::string name = peekToken();

    validateServerName(config, name);

    config.setServerName(name);
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

void ConfigParser::parseRoot(ServerConfig &config) {
  consumeToken("root");

  validateHasValue("root");
  validateNotDuplicate("root", !config.getRoot().empty());

  config.setRoot(peekToken());
  this->tokenPosition++;

  validateOnlyOneValue("root");
  validateSemicolon();
}

void ConfigParser::validateHasValue(const std::string &directive) const {
  if (!hasMoreTokens() || isDelimiter(peekToken()[0])) {
    throw std::runtime_error("Directive '" + directive + "' requires a value.");
  }
}

void ConfigParser::validateOnlyOneValue(const std::string &directive) const {
  if (hasMoreTokens() && !isDelimiter(peekToken()[0])) {
    throw std::runtime_error("Directive '" + directive +
                             "' requires exactly one value.");
  }
}

void ConfigParser::validateNotDuplicate(const std::string &directive,
                                        bool isSet) const {
  if (isSet) {
    throw std::runtime_error("Duplicate '" + directive + "' directive found.");
  }
}

void ConfigParser::parseClientMaxBodySize(ServerConfig &config) {
  consumeToken("client_max_body_size");

  validateHasValue("client_max_body_size");
  validateNotDuplicate("client_max_body_size",
                       config.getClientMaxBodySize() != -1);

  long validatedValue = validateMaxBodySize(peekToken());
  config.setClientMaxBodySize(validatedValue);
  this->tokenPosition++;

  validateOnlyOneValue("client_max_body_size");
  validateSemicolon();
}

long ConfigParser::validateMaxBodySize(const std::string &rawValue) const {
  std::string numberPart = getNumberPart(rawValue);
  long multiplier = getMultiplier(rawValue);

  if (!isFullNumber(numberPart)) {
    throw std::runtime_error("Invalid number for client_max_body_size: " +
                             numberPart);
  }

  long size = std::atol(numberPart.c_str());
  return size * multiplier;
}

std::string ConfigParser::getNumberPart(const std::string &rawValue) const {
  char lastChar = rawValue[rawValue.length() - 1];
  if (!std::isdigit(lastChar)) {
    return rawValue.substr(0, rawValue.length() - 1);
  }
  return rawValue;
}

long ConfigParser::getMultiplier(const std::string &rawValue) const {
  char lastChar = rawValue[rawValue.length() - 1];
  if (!std::isdigit(lastChar)) {
    if (lastChar == 'k' || lastChar == 'K') {
      return 1024;
    }
    if (lastChar == 'm' || lastChar == 'M') {
      return 1024 * 1024;
    }
    throw std::runtime_error("Invalid suffix for client_max_body_size: " +
                             std::string(1, lastChar));
  }
  return 1;
}

void ConfigParser::parseIndex(ServerConfig &config) {
  consumeToken("index");
  validateHasValue("index");

  validateNotDuplicate("index", !config.getIndexFiles().empty());

  while (hasMoreTokens() && !isDelimiter(peekToken()[0])) {
    const std::string &file = peekToken();
    config.setIndexFile(file);
    this->tokenPosition++;
  }
  validateSemicolon();
}

void ConfigParser::validateSemicolon() { consumeToken(";"); }

void ConfigParser::skipComment(const std::string &content, size_t &index) {
  while (index < content.length() && content[index] != '\n') {
    index++;
  }
}

bool ConfigParser::isDelimiter(char c) const {
  return (c == '{' || c == '}' || c == ';');
}

void ConfigParser::pushCurrentToken(std::string &token) {
  if (!token.empty()) {
    this->tokens.push_back(token);
    token.clear();
  }
}

bool ConfigParser::isWhitespace(char c) const {
  return std::isspace(static_cast<unsigned char>(c));
}
