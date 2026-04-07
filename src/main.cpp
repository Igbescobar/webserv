/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: igngonza <igngonza@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 11:25:32 by igngonza          #+#    #+#             */
/*   Updated: 2026/04/07 17:16:16 by igngonza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Parser.hpp"
#include <iostream>
#include <string>

void printAllConfigs(const std::vector<ServerConfig> &servers) {
  std::cout << "\n✅ Configuration file parsed successfully!" << std::endl;
  std::cout << "Found " << servers.size() << " server blocks." << std::endl;
  std::cout << "========================================\n" << std::endl;

  for (size_t i = 0; i < servers.size(); ++i) {
    const ServerConfig &server = servers[i];
    std::cout << "Server #" << i + 1 << std::endl;

    // Print Listen directives
    std::cout << "  Listen: ";
    const std::vector<std::string> &listens = server.getListen();
    for (size_t j = 0; j < listens.size(); ++j) {
      std::cout << listens[j] << " ";
    }
    std::cout << std::endl;

    // Print Server Names
    std::cout << "  Server Names: ";
    const std::vector<std::string> &names = server.getServerNames();
    for (size_t j = 0; j < names.size(); ++j) {
      std::cout << names[j] << " ";
    }
    std::cout << std::endl;

    // Print Locations
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
        const std::vector<std::string> &methods = loc.getAllowedMethods();
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
      std::cout << "      Autoindex: " << (loc.isAutoIndex() ? "on" : "off")
                << std::endl;
    }
    std::cout << "========================================\n" << std::endl;
  }
}

int main(int argc, char **argv) {
  if (argc > 2) {
    std::cerr << "Usage: " << argv[0] << " [configuration_file]" << std::endl;
    return 1;
  }

  std::string config_path = (argc == 2) ? argv[1] : "config/default.conf";

  try {
    Parser parser;

    std::vector<ServerConfig> servers = parser.parse(config_path);

    printAllConfigs(servers);

  } catch (const std::exception &e) {
    // If parsing fails, the exception will be caught here
    std::cerr << "\nError during parsing: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}