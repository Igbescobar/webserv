#include "parser/config/ConfigParser.hpp"
#include "parser/config/LocationConfig.hpp"
#include "parser/config/ServerConfig.hpp"
#include <stdexcept>

void ConfigParser::parseLocation(ServerConfig &config) {
  consumeToken("location");

  validateHasValue("location");

  LocationConfig location;
  location.setMatchType(LocationConfig::PREFIX);

  location.setPattern(peekToken());
  tokenPosition++;

  consumeToken("{");

  while (hasMoreTokens() && peekToken() != "}") {
    parseLocationDirective(location);
  }

  consumeToken("}");
  config.addLocation(location);
}

void ConfigParser::parseLocationDirective(LocationConfig &location) {
  const std::string &directive = peekToken();

  if (directive == "root") {
    parseLocationRoot(location);
  } else if (directive == "index") {
    parseLocationIndex(location);
  } else if (directive == "autoindex") {
    parseLocationAutoIndex(location);
  } else if (directive == "allow_methods") {
    parseLocationAllowedMethods(location);
  } else if (directive == "return") {
    parseLocationReturn(location);
  } else if (directive == "upload_path") {
    parseLocationUploadPath(location);
  } else if (directive == "client_max_body_size") {
    parseLocationClientMaxBodySize(location);
  } else {
    throw std::runtime_error("Invalid location directive: " + directive);
  }
}