#include "parser/config/ConfigParser.hpp"
#include "parser/config/ServerConfig.hpp"
#include <cstdlib>
#include <stdexcept>

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