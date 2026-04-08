/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: igngonza <igngonza@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 14:18:20 by igngonza          #+#    #+#             */
/*   Updated: 2026/04/08 15:33:58 by igngonza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Parser.hpp"
#include <climits>
#include <cstdlib>
#include <iostream>

static bool isValidMethod(const std::string &method);
static bool isNumericToken(const std::string &token);
static bool isValidIpv4(const std::string &host);
static bool isValidHostname(const std::string &host);
static bool isValidListenHost(const std::string &host);
static bool isValidPort(const std::string &portToken);
static void validateListenValue(const std::string &listenValue);
static long parseBodySize(const std::string &token);

Parser::Parser() : _currentTokenIndex(0) {
}

Parser::~Parser() {
}

void Parser::expectToken(const std::string &expected,
                         const std::string &error) {
    if (getNextToken() != expected)
        throw std::runtime_error(error);
}

void Parser::expectSemicolon(const std::string &directiveName) {
    expectToken(";", "Error: Missing ';' after " + directiveName + ".");
}

void Parser::tokenizeFile(const std::string &filename) {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open configuration file: " +
                                 filename);
    }

    std::stringstream contentStream;
    contentStream << file.rdbuf();
    std::string content = contentStream.str();
    file.close();

    std::string currentToken;
    for (size_t i = 0; i < content.length(); ++i) {
        char c = content[i];
        if (c == '#') {
            while (i < content.length() && content[i] != '\n')
                i++;
            continue;
        }
        if (c == '{' || c == '}' || c == ';') {
            if (!currentToken.empty())
                _tokens.push_back(currentToken);
            currentToken.clear();
            _tokens.push_back(std::string(1, c));
            continue;
        }
        if (std::isspace(static_cast<unsigned char>(c))) {
            if (!currentToken.empty())
                _tokens.push_back(currentToken);
            currentToken.clear();
        } else {
            currentToken += c;
        }
    }
    if (!currentToken.empty())
        _tokens.push_back(currentToken);
}

std::vector<ServerConfig> Parser::parse(const std::string &filename) {
    _tokens.clear();
    _currentTokenIndex = 0;
    tokenizeFile(filename);
    std::vector<ServerConfig> servers;

    while (hasMoreTokens()) {
        if (peekNextToken() == "server") {
            getNextToken();
            parseServerBlock(servers);
        } else {
            throw std::runtime_error("Error: Expected 'server' keyword.");
        }
    }
    return servers;
}

void Parser::parseServerBlock(std::vector<ServerConfig> &servers) {
    ServerConfig currentServer;

    expectToken("{", "Error: Expected '{' after 'server' keyword.");

    while (hasMoreTokens() && peekNextToken() != "}") {
        const std::string directive = getNextToken();
        parseServerDirective(directive, currentServer);
    }

    expectToken("}", "Error: Expected '}' to close server block.");
    validateServerSemantics(currentServer);

    servers.push_back(currentServer);
}

void Parser::validateServerSemantics(const ServerConfig &server) const {
    if (server.getListen().empty()) {
        throw std::runtime_error(
            "Error: Each server block must contain at least one listen "
            "directive.");
    }
}

void Parser::parseLocationBlock(ServerConfig &server,
                                const ServerConfig &serverDefaults) {
    LocationConfig currentLocation;

    currentLocation.setRoot(serverDefaults.getRoot());
    currentLocation.setAutoIndex(serverDefaults.isAutoIndex());
    currentLocation.setClientMaxBodySize(serverDefaults.getClientMaxBodySize());
    for (size_t i = 0; i < serverDefaults.getIndexFiles().size(); ++i) {
        currentLocation.addIndexFile(serverDefaults.getIndexFiles()[i]);
    }

    currentLocation.setPath(getNextToken());

    expectToken("{", "Error: Expected '{' after location path.");

    while (hasMoreTokens() && peekNextToken() != "}") {
        const std::string directive = getNextToken();
        parseLocationDirective(directive, server, currentLocation);
    }

    expectToken("}", "Error: Expected '}' to close location block.");
    server.addLocation(currentLocation);
}

void Parser::parseServerNames(ServerConfig &server) {
    while (peekNextToken() != ";") {
        server.addServerName(getNextToken());
    }
    expectSemicolon("server_name");
}

void Parser::parseIndexDirective(LocationConfig &location) {
    location.getIndexFiles().clear();
    while (peekNextToken() != ";") {
        location.addIndexFile(getNextToken());
    }
    expectSemicolon("index");
}

void Parser::parseServerIndexDirective(ServerConfig &server) {
    server.clearIndexFiles();
    while (peekNextToken() != ";") {
        server.addIndexFile(getNextToken());
    }
    expectSemicolon("index");
}

void Parser::parseErrorPageDirective(ServerConfig &server) {
    std::vector<int> codes;

    while (hasMoreTokens() && peekNextToken() != ";") {
        const std::string token = getNextToken();
        if (peekNextToken() == ";") {
            if (codes.empty()) {
                throw std::runtime_error(
                    "Error: error_page needs at least one status code.");
            }
            for (size_t i = 0; i < codes.size(); ++i)
                server.addErrorPage(codes[i], token);
            break;
        }

        if (!isNumericToken(token)) {
            throw std::runtime_error(
                "Error: Invalid error code in error_page: " + token);
        }
        codes.push_back(std::atoi(token.c_str()));
    }

    expectSemicolon("error_page");
}

void Parser::parseServerDirective(const std::string &directive,
                                  ServerConfig &server) {
    if (directive == "listen") {
        const std::string listenValue = getNextToken();
        validateListenValue(listenValue);
        server.addListen(listenValue);
        expectSemicolon("listen");
        return;
    }
    if (directive == "server_name") {
        parseServerNames(server);
        return;
    }
    if (directive == "root") {
        server.setRoot(getNextToken());
        expectSemicolon("root");
        return;
    }
    if (directive == "client_max_body_size") {
        server.setClientMaxBodySize(parseBodySize(getNextToken()));
        expectSemicolon("client_max_body_size");
        return;
    }
    if (directive == "autoindex") {
        const std::string value = getNextToken();
        if (value == "on")
            server.setAutoIndex(true);
        else if (value == "off")
            server.setAutoIndex(false);
        else
            throw std::runtime_error(
                "Error: autoindex value must be 'on' or 'off'.");
        expectSemicolon("autoindex");
        return;
    }
    if (directive == "index") {
        parseServerIndexDirective(server);
        return;
    }
    if (directive == "error_page") {
        parseErrorPageDirective(server);
        return;
    }
    if (directive == "location") {
        parseLocationBlock(server, server);
        return;
    }

    throw std::runtime_error("Error: Unknown directive in server block: " +
                             directive);
}

void Parser::parseLocationBlock(ServerConfig &server,
                                const LocationConfig &defaults) {
    LocationConfig currentLocation = defaults;

    currentLocation.setPath(getNextToken());

    expectToken("{", "Error: Expected '{' after location path.");

    while (hasMoreTokens() && peekNextToken() != "}") {
        const std::string directive = getNextToken();
        parseLocationDirective(directive, server, currentLocation);
    }

    expectToken("}", "Error: Expected '}' to close location block.");
    server.addLocation(currentLocation);
}

void Parser::parseAllowMethodsDirective(LocationConfig &location) {
    location.getAllowedMethods().clear();
    while (peekNextToken() != ";") {
        const std::string method = getNextToken();
        if (!isValidMethod(method)) {
            throw std::runtime_error(
                "Error: Invalid HTTP method in allow_methods: " + method);
        }
        location.addAllowedMethod(method);
    }
    expectSemicolon("allow_methods");
}

void Parser::parseReturnDirective(LocationConfig &location) {
    const std::string codeToken = getNextToken();
    if (!isNumericToken(codeToken)) {
        throw std::runtime_error("Error: Invalid return status code: " +
                                 codeToken);
    }
    const int code = std::atoi(codeToken.c_str());
    const std::string target = getNextToken();
    location.setReturn(code, target);
    expectSemicolon("return");
}

void Parser::parseLocationDirective(const std::string &directive,
                                    ServerConfig &server,
                                    LocationConfig &location) {
    if (directive == "index") {
        parseIndexDirective(location);
        return;
    }
    if (directive == "allow_methods") {
        parseAllowMethodsDirective(location);
        return;
    }
    if (directive == "root") {
        location.setRoot(getNextToken());
        expectSemicolon("root");
        return;
    }
    if (directive == "client_max_body_size") {
        location.setClientMaxBodySize(parseBodySize(getNextToken()));
        expectSemicolon("client_max_body_size");
        return;
    }
    if (directive == "autoindex") {
        const std::string value = getNextToken();
        if (value == "on")
            location.setAutoIndex(true);
        else if (value == "off")
            location.setAutoIndex(false);
        else
            throw std::runtime_error(
                "Error: autoindex value must be 'on' or 'off'.");
        expectSemicolon("autoindex");
        return;
    }
    if (directive == "upload_path") {
        location.setUploadPath(getNextToken());
        expectSemicolon("upload_path");
        return;
    }
    if (directive == "return") {
        parseReturnDirective(location);
        return;
    }
    if (directive == "cgi_pass") {
        const std::string extension = getNextToken();
        const std::string binaryPath = getNextToken();
        location.addCgiPass(extension, binaryPath);
        expectSemicolon("cgi_pass");
        return;
    }
    if (directive == "location") {
        parseLocationBlock(server, location);
        return;
    }

    throw std::runtime_error("Error: Unknown directive in location block: " +
                             directive);
}

bool Parser::hasMoreTokens() const {
    return _currentTokenIndex < _tokens.size();
}

const std::string &Parser::getNextToken() {
    if (!hasMoreTokens())
        throw std::runtime_error("Error: Unexpected end of file.");
    return _tokens[_currentTokenIndex++];
}

const std::string &Parser::peekNextToken() const {
    if (!hasMoreTokens())
        throw std::runtime_error("Error: Unexpected end of file.");
    return _tokens[_currentTokenIndex];
}

static bool isValidMethod(const std::string &method) {
    return method == "GET" || method == "POST" || method == "DELETE";
}

static bool isNumericToken(const std::string &token) {
    if (token.empty())
        return false;
    for (size_t i = 0; i < token.size(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(token[i])))
            return false;
    }
    return true;
}

static bool isValidIpv4(const std::string &host) {
    size_t start = 0;
    int parts = 0;

    while (start < host.size()) {
        size_t dotPos = host.find('.', start);
        std::string part = host.substr(start, dotPos - start);

        if (!isNumericToken(part))
            return false;

        char *end = NULL;
        long value = std::strtol(part.c_str(), &end, 10);
        if (*end != '\0' || value < 0 || value > 255)
            return false;

        ++parts;
        if (dotPos == std::string::npos)
            break;
        start = dotPos + 1;
    }
    return parts == 4;
}

static bool isValidHostname(const std::string &host) {
    if (host.empty() || host[0] == '.' || host[host.size() - 1] == '.')
        return false;

    for (size_t i = 0; i < host.size(); ++i) {
        const char c = host[i];
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '.' || c == '-')
            continue;
        return false;
    }
    return true;
}

static bool isValidListenHost(const std::string &host) {
    return host == "localhost" || host == "*" || isValidIpv4(host) ||
           isValidHostname(host);
}

static bool isValidPort(const std::string &portToken) {
    if (!isNumericToken(portToken))
        return false;

    char *end = NULL;
    long value = std::strtol(portToken.c_str(), &end, 10);
    return *end == '\0' && value >= 1 && value <= 65535;
}

static void validateListenValue(const std::string &listenValue) {
    const size_t colonPos = listenValue.rfind(':');

    if (colonPos == std::string::npos) {
        if (!isValidPort(listenValue)) {
            throw std::runtime_error("Error: Invalid listen port: " +
                                     listenValue);
        }
        return;
    }

    const std::string host = listenValue.substr(0, colonPos);
    const std::string port = listenValue.substr(colonPos + 1);

    if (host.empty() || port.empty() || host.find(':') != std::string::npos) {
        throw std::runtime_error("Error: Invalid listen value: " + listenValue);
    }
    if (!isValidListenHost(host)) {
        throw std::runtime_error("Error: Invalid listen host: " + host);
    }
    if (!isValidPort(port)) {
        throw std::runtime_error("Error: Invalid listen port: " + port);
    }
}

static long parseBodySize(const std::string &token) {
    if (token.empty())
        throw std::runtime_error(
            "Error: client_max_body_size cannot be empty.");

    std::string numberPart = token;
    long multiplier = 1;
    const char suffix = token[token.size() - 1];

    if (!std::isdigit(static_cast<unsigned char>(suffix))) {
        numberPart = token.substr(0, token.size() - 1);
        if (numberPart.empty()) {
            throw std::runtime_error(
                "Error: Invalid client_max_body_size value: " + token);
        }
        if (suffix == 'k' || suffix == 'K')
            multiplier = 1024;
        else if (suffix == 'm' || suffix == 'M')
            multiplier = 1024 * 1024;
        else if (suffix == 'g' || suffix == 'G')
            multiplier = 1024 * 1024 * 1024;
        else
            throw std::runtime_error("Error: Invalid size suffix in: " + token);
    }

    if (!isNumericToken(numberPart)) {
        throw std::runtime_error("Error: Invalid client_max_body_size value: " +
                                 token);
    }

    char *end = NULL;
    const long base = std::strtol(numberPart.c_str(), &end, 10);
    if (*end != '\0' || base < 0)
        throw std::runtime_error("Error: Invalid client_max_body_size value: " +
                                 token);
    if (base > LONG_MAX / multiplier)
        throw std::runtime_error("Error: client_max_body_size is too large: " +
                                 token);
    return base * multiplier;
}