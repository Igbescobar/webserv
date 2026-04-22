#include "../../../inc/parser/config/ConfigParser.hpp"

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

void ConfigParser::validateSemicolon() { consumeToken(";"); }