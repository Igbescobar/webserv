/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: igngonza <igngonza@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 14:17:18 by igngonza          #+#    #+#             */
/*   Updated: 2026/04/07 17:06:54 by igngonza         ###   ########.fr       */
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
  void parseLocationBlock(ServerConfig &server, const LocationConfig &defaults);

  const std::string &getNextToken();
  const std::string &peekNextToken() const;
  bool hasMoreTokens() const;
};

#endif