#include "../../../inc/parser/config/ConfigParser.hpp"

void ConfigParser::parseIndex(ServerConfig &config) {
  consumeToken("index");
  validateHasValue("index");

  validateNotDuplicate("index", !config.getIndexFiles().empty());

  while (hasMoreTokens() && !isDelimiter(peekToken()[0])) {
    const std::string &file = peekToken();
    config.addIndexFile(file);
    this->tokenPosition++;
  }
  validateSemicolon();
}