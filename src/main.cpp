/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: igngonza <igngonza@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 11:25:32 by igngonza          #+#    #+#             */
/*   Updated: 2026/04/08 12:58:03 by igngonza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Parser.hpp"
#include <iostream>
#include <string>

static void
printServerErrorPages(const std::map<int, std::string> &errorPages) {
    if (errorPages.empty())
        return;

    std::cout << "  Error Pages:" << std::endl;
    for (std::map<int, std::string>::const_iterator it = errorPages.begin();
         it != errorPages.end(); ++it) {
        std::cout << "    " << it->first << " -> " << it->second << std::endl;
    }
}

static void
printLocationCgiPass(const std::map<std::string, std::string> &cgiPass) {
    if (cgiPass.empty())
        return;

    std::cout << "      CGI Pass:" << std::endl;
    for (std::map<std::string, std::string>::const_iterator it =
             cgiPass.begin();
         it != cgiPass.end(); ++it) {
        std::cout << "        " << it->first << " -> " << it->second
                  << std::endl;
    }
}

void printAllConfigs(const std::vector<ServerConfig> &servers) {
    std::cout << "Found " << servers.size() << " server blocks." << std::endl;
    std::cout << "========================================\n" << std::endl;

    for (size_t i = 0; i < servers.size(); ++i) {
        const ServerConfig &server = servers[i];
        std::cout << "Server #" << i + 1 << std::endl;

        std::cout << "  Listen: ";
        const std::vector<std::string> &listens = server.getListen();
        for (size_t j = 0; j < listens.size(); ++j) {
            std::cout << listens[j] << " ";
        }
        std::cout << std::endl;

        std::cout << "  Server Names: ";
        const std::vector<std::string> &names = server.getServerNames();
        for (size_t j = 0; j < names.size(); ++j) {
            std::cout << names[j] << " ";
        }
        std::cout << std::endl;

        if (server.getClientMaxBodySize() != -1) {
            std::cout << "  Client Max Body Size: "
                      << server.getClientMaxBodySize() << std::endl;
        }
        if (!server.getRoot().empty()) {
            std::cout << "  Root: " << server.getRoot() << std::endl;
        }
        if (!server.getIndexFiles().empty()) {
            std::cout << "  Index: ";
            const std::vector<std::string> &serverIndexes =
                server.getIndexFiles();
            for (size_t j = 0; j < serverIndexes.size(); ++j) {
                std::cout << serverIndexes[j] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "  Autoindex: " << (server.isAutoIndex() ? "on" : "off")
                  << std::endl;
        printServerErrorPages(server.getErrorPages());

        std::cout << "  Locations: " << std::endl;
        const std::vector<LocationConfig> &locations = server.getLocations();
        for (size_t j = 0; j < locations.size(); ++j) {
            const LocationConfig &loc = locations[j];
            std::cout << "    ---------------------" << std::endl;
            std::cout << "    Location Path: " << loc.getPath() << std::endl;
            if (!loc.getRoot().empty()) {
                std::cout << "      Root: " << loc.getRoot() << std::endl;
            }
            if (!loc.getAllowedMethods().empty()) {
                std::cout << "      Allowed Methods: ";
                const std::vector<std::string> &methods =
                    loc.getAllowedMethods();
                for (size_t k = 0; k < methods.size(); ++k) {
                    std::cout << methods[k] << " ";
                }
                std::cout << std::endl;
            }
            if (!loc.getIndexFiles().empty()) {
                std::cout << "      Index: ";
                const std::vector<std::string> &indexes = loc.getIndexFiles();
                for (size_t k = 0; k < indexes.size(); ++k) {
                    std::cout << indexes[k] << " ";
                }
                std::cout << std::endl;
            }
            if (loc.getClientMaxBodySize() != -1) {
                std::cout << "      Client Max Body Size: "
                          << loc.getClientMaxBodySize() << std::endl;
            }
            if (!loc.getUploadPath().empty()) {
                std::cout << "      Upload Path: " << loc.getUploadPath()
                          << std::endl;
            }
            if (loc.getReturn().first != 0 || !loc.getReturn().second.empty()) {
                std::cout << "      Return: " << loc.getReturn().first << " "
                          << loc.getReturn().second << std::endl;
            }
            printLocationCgiPass(loc.getCgiPass());
            std::cout << "      Autoindex: "
                      << (loc.isAutoIndex() ? "on" : "off") << std::endl;
        }
        std::cout << "========================================\n" << std::endl;
    }
}

int main(int argc, char **argv) {
    if (argc > 2) {
        std::cerr << "Usage: " << argv[0] << " [configuration_file]"
                  << std::endl;
        return 1;
    }

    std::string config_path = (argc == 2) ? argv[1] : "config/default.conf";

    try {
        Parser parser;

        std::vector<ServerConfig> servers = parser.parse(config_path);

        printAllConfigs(servers);

    } catch (const std::exception &e) {
        std::cerr << "\nError during parsing: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}