#include "parser/config/ConfigParser.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

void ConfigParser::tokenizeFile(const std::string &filename) {
  std::string rawContent = readFileContents(filename);

  generateTokens(rawContent);
}

std::string ConfigParser::readFileContents(const std::string &filename) {
  std::ifstream file(filename.c_str());
  if (!file.is_open()) {
    throw std::runtime_error("Error: Could not open config file: " + filename);
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

void ConfigParser::generateTokens(const std::string &content) {
  std::string currentToken;

  for (size_t i = 0; i < content.length(); i++) {
    char c = content[i];

    if (c == '#') {
      skipComment(content, i);
    } else if (isDelimiter(c)) {
      pushCurrentToken(currentToken);
      this->tokens.push_back(std::string(1, c));
    } else if (isWhitespace(c))
      pushCurrentToken(currentToken);
    else
      currentToken += c;
  }
}

bool ConfigParser::hasMoreTokens() const {
  return this->tokenPosition < this->tokens.size();
}

const std::string &ConfigParser::peekToken() const {
  if (!this->hasMoreTokens()) {
    throw std::runtime_error("Unexpected end of configuration file.");
  }
  return this->tokens[this->tokenPosition];
}

void ConfigParser::consumeToken(const std::string &expected) {
  if (this->peekToken() != expected) {
    throw std::runtime_error("Expected '" + expected + "' but found '" +
                             this->peekToken() + "'");
  }
  this->tokenPosition++;
}

void ConfigParser::pushCurrentToken(std::string &token) {
  if (!token.empty()) {
    this->tokens.push_back(token);
    token.clear();
  }
}