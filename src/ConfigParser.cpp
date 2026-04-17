#include "ConfigParser.hpp"

void ConfigParser::tokenizeFile(const std::string &filename) {
	std::string rawContent = readFileContents(filename);
	
	generateTokens(rawContent);
}

std::string ConfigParser::readFileContents(const std::string &filemane) {
	std::ifstream file(filename.c_str());
	if (!file.is_open()) {
		throw std::runtime_error("Error: Could not open config file: " + filename;	
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

void ConfigParser::generateTokens(const std::string &content) {
	std::string currentToken;

	for (size_t = 0; i < content.length(); i++) {
	char c = content[i];

	if (c == '#') 
		skipComment(content, i);
	else if (isDelimiter(c)) {
		pushCurrentToken(currentToken);
		this->tokens.push_back(std::string(1, c));
	} else if (isWhiteSpace(c) 
		pushCurrentToken(currentToken);
	else 
		currentToken += c;
}

void ConfigParser::skipComment(const std::string &content, size_t &index) {
    while (index < content.length() && content[index] != '\n') {
        index++;
    }
}

bool ConfigParser::isDelimiter(char c) const {
    return (c == '{' || c == '}' || c == ';');
}

void ConfigParser::pushCurrentToken(std::string &token) {
    if (!token.empty()) {
        this->tokens.push_back(token);
        token.clear();
    }
}

bool ConfigParser::isWhitespace(char c) const {
    return std::isspace(static_cast<unsigned char>(c));
}
	

	
