#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

# include <iostream>
# include <vector>
# include <string>
# include <fstream>
# include <sstream>
# include <stdexcept>
# include <cctype>

class ConfigParser {
public:
    ConfigParser();
    ~ConfigParser();

    void tokenizeFile(const std::string &filename);
    
    const std::vector<std::string>& getTokens() const;

private:
    std::vector<std::string> tokens;

    void skipComment(const std::string &content, size_t &index);
    void pushCurrentToken(std::string &token);
    bool isDelimiter(char c) const;
    bool isWhitespace(char c) const;
};

#endif
