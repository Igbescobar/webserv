/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: igngonza <igngonza@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 14:17:18 by igngonza          #+#    #+#             */
/*   Updated: 2026/04/08 15:34:00 by igngonza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include "../inc/ServerConfig.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

class Parser {
  public:
    Parser();
    ~Parser();

    std::vector<ServerConfig> parse(const std::string &filename);

  private:
    std::vector<std::string> _tokens;
    size_t _currentTokenIndex;

    void tokenizeFile(const std::string &filename);
    void parseServerBlock(std::vector<ServerConfig> &servers);
    void parseLocationBlock(ServerConfig &server,
                            const ServerConfig &serverDefaults);
    void parseLocationBlock(ServerConfig &server,
                            const LocationConfig &defaults);
    void parseServerDirective(const std::string &directive,
                              ServerConfig &server);
    void parseLocationDirective(const std::string &directive,
                                ServerConfig &server, LocationConfig &location);

    void parseServerNames(ServerConfig &server);
    void parseServerIndexDirective(ServerConfig &server);
    void parseIndexDirective(LocationConfig &location);
    void parseAllowMethodsDirective(LocationConfig &location);
    void parseErrorPageDirective(ServerConfig &server);
    void parseReturnDirective(LocationConfig &location);
    void validateServerSemantics(const ServerConfig &server) const;

    void expectToken(const std::string &expected, const std::string &error);
    void expectSemicolon(const std::string &directiveName);

    const std::string &getNextToken();
    const std::string &peekNextToken() const;
    bool hasMoreTokens() const;
};

#endif