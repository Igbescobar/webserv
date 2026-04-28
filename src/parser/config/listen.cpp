#include "parser/config/ConfigParser.hpp"
#include "parser/config/ServerConfig.hpp"
#include <sstream>
#include <stdexcept>

void ConfigParser::parseListen(ServerConfig &config) {
  consumeToken("listen");

  validateHasValue("listen");

  std::string rawValue = peekToken();
  this->tokenPosition++;

  std::string host = extractHost(rawValue);
  std::string portStr = extractPort(rawValue);

  host = normalizeHost(host);
  validateIP(host);
  validatePort(portStr);

  int port = std::atoi(portStr.c_str());

  config.addIP(host);
  config.addPort(port);
  config.addListen(host + ":" + portStr);
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
  if (port < 1024 || port > 65535) {
    throw std::runtime_error("Port out of range (1024-65535): " + portStr);
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
