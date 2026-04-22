#include "../../../inc/parser/config/ConfigParser.hpp"
#include "../../../inc/parser/config/ServerConfig.hpp"

void ConfigParser::parseErrorPage(ServerConfig &config) {
  consumeToken("error_page");

  std::vector<int> codes = collectErrorCodes();
  std::string page = getErrorPagePath();

  for (size_t i = 0; i < codes.size(); i++)
    config.addErrorPage(codes[i], page);

  validateSemicolon();
}

std::vector<int> ConfigParser::collectErrorCodes() {
  std::vector<int> codes;
  while (hasMoreTokens() && isFullNumber(peekToken())) {
    codes.push_back(validateAndParseErrorCode(peekToken()));
    this->tokenPosition++;
  }
  if (codes.empty())
    throw std::runtime_error(
        "Directive error_pages requires at least one error code");
  return codes;
}

int ConfigParser::validateAndParseErrorCode(const std::string &codeStr) const {
  if (!isFullNumber(codeStr))
    throw std::runtime_error("Invalid error code: " + codeStr);
  long code = atol(codeStr.c_str());
  if (code < 300 || code > 599) {
    throw std::runtime_error("Error code out of range (300 - 599): " + codeStr);
  }
  return static_cast<int>(code);
}

std::string ConfigParser::getErrorPagePath() {
  validateHasValue("error_page path");
  const std::string &page = peekToken();
  this->tokenPosition++;
  return page;
}