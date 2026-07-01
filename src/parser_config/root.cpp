#include "parser_config/ConfigParser.hpp"
#include "parser_config/ServerConfig.hpp"

void ConfigParser::parseRoot(ServerConfig &config) {
  consumeToken("root");

  validateHasValue("root");
  validateNotDuplicate("root", !config.getRoot().empty());

  config.setRoot(peekToken());
  this->tokenPosition++;

  validateOnlyOneValue("root");
  validateSemicolon();
}