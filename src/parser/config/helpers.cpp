#include "../../../inc/parser/config/ConfigParser.hpp"

void ConfigParser::skipComment(const std::string &content, size_t &index) {
  while (index < content.length() && content[index] != '\n') {
    index++;
  }
}

bool ConfigParser::isDelimiter(char c) const {
  return (c == '{' || c == '}' || c == ';');
}

bool ConfigParser::isWhitespace(char c) const {
  return std::isspace(static_cast<unsigned char>(c));
}