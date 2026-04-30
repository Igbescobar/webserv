#include "parser/config/ConfigParser.hpp"
#include "parser/config/LocationConfig.hpp"
#include <stdexcept>

void ConfigParser::parseLocationRoot(LocationConfig &location) {
  consumeToken("root");
  validateHasValue("root");
  location.setRoot(peekToken());
  tokenPosition++;
  validateSemicolon();
}

void ConfigParser::parseLocationIndex(LocationConfig &location) {
  consumeToken("index");
  validateHasValue("index");
  while (hasMoreTokens() && peekToken() != ";") {
    location.addIndex(peekToken());
    tokenPosition++;
  }
  validateSemicolon();
}

void ConfigParser::parseLocationAutoIndex(LocationConfig &location) {
  consumeToken("autoindex");
  validateHasValue("autoindex");

  const std::string value = peekToken();
  tokenPosition++;

  if (value == "on")
    location.setAutoIndex(true);
  else if (value == "off")
    location.setAutoIndex(false);
  else
    throw std::runtime_error("autoindex must be 'on' or 'off'");

  validateSemicolon();
}

void ConfigParser::parseLocationAllowedMethods(LocationConfig &location) {
  consumeToken("allow_methods");
  validateHasValue("allow_methods");

  while (hasMoreTokens() && peekToken() != ";") {
    location.addAllowedMethod(peekToken());
    tokenPosition++;
  }
  validateSemicolon();
}

void ConfigParser::parseLocationReturn(LocationConfig &location) {
  consumeToken("return");
  validateHasValue("return");

  std::string target;
  while (hasMoreTokens() && peekToken() != ";") {
    if (!target.empty())
      target += " ";
    target += peekToken();
    tokenPosition++;
  }
  location.setReturnTarget(target);
  validateSemicolon();
}

void ConfigParser::parseLocationUploadPath(LocationConfig &location) {
  consumeToken("upload_path");
  validateHasValue("upload_path");
  location.setUploadPath(peekToken());
  tokenPosition++;
  validateSemicolon();
}

void ConfigParser::parseLocationClientMaxBodySize(LocationConfig &location) {
  consumeToken("client_max_body_size");

  validateHasValue("client_max_body_size");
  validateNotDuplicate("client_max_body_size",
                       location.getClientMaxBodySize() != -1);

  long validatedValue = validateMaxBodySize(peekToken());
  location.setClientMaxBodySize(validatedValue);
  this->tokenPosition++;

  validateOnlyOneValue("client_max_body_size");
  validateSemicolon();
}

void ConfigParser::parseLocationCgiPass(LocationConfig &location) {
  consumeToken("cgi_pass");
  validateHasValue("cgi_pass");
  validateNotDuplicate("cgi_pass", !location.getCgiPassExtensions().empty());

  validateNotDuplicate("cgi_pass", !location.getCgiPassExtensions().empty());

  // Accept multiple extensions until ';'
  while (hasMoreTokens() && peekToken() != ";") {
    const std::string ext = peekToken();
    tokenPosition++;
    validateCgiPassExtension(ext);
    location.addCgiPassExtensions(ext);
  }

  validateSemicolon();
}