#include "../../../inc/parser/config/ConfigParser.hpp"
#include "../../../inc/parser/config/ServerConfig.hpp"

void ConfigParser::parseRoot(ServerConfig &config) {
  consumeToken("root");

  validateHasValue("root");
  validateNotDuplicate("root", !config.getRoot().empty());

  config.setRoot(peekToken());
  this->tokenPosition++;

  validateOnlyOneValue("root");
  validateSemicolon();
}