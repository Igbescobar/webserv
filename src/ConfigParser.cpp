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
  const std::string &directive = this->peekToken();

  if (directive == "listen") {
    parseListen(config);
  } else {
    throw std::runtime_error("Invalid server directive: " + directive);
  }
}

void ConfigParser::parseListen(ServerConfig &config) {
  consumeToken("listen");

  if (!hasMoreTokens() || isDelimiter(peekToken()[0])) {
    throw std::runtime_error("Directive 'listen' requires a value.");
  }

  std::string rawValue = peekToken();
  this->tokenPosition++;

  std::string normalizedValue = normalizeListen(rawValue);

  config.addListen(normalizedValue);

  validateSemicolon();
}

std::string ConfigParser::normalizeListen(const std::string &raw) {
  std::string host = "0.0.0.0";
  std::string port = "8080";
  size_t colonPos = raw.find(':');

  if (colonPos != std::string::npos) {
    host = raw.substr(0, colonPos);
    port = raw.substr(colonPos + 1);
  } else if (isFullNumber(raw)) {
    port = raw;
  } else {
    host = raw;
  }

  if (host == "localhost") {
    host = "127.0.0.1";
  }

  validateIP(host);
  validatePort(port);

  return host + ":" + port;
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
