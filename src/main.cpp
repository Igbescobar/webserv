/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: igngonza <igngonza@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 11:25:32 by igngonza          #+#    #+#             */
/*   Updated: 2026/04/16 12:32:33 by igngonza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Parser.hpp"
#include "../inc/Server.hpp"
#include <iostream>
#include <map>
#include <string>
#include <vector>

int main(int argc, char **argv) {
    if (argc > 2) {
        std::cerr << "Usage: " << argv[0] << " [configuration_file]"
                  << std::endl;
        return 1;
    }

    std::string config_path = (argc == 2) ? argv[1] : "config/default.conf";

    try {
        Parser parser;
        std::vector<ServerConfig> configs = parser.parse(config_path);

        if (configs.empty()) {
            std::cerr << "Error: No server configuration found in "
                      << config_path << std::endl;
            return 1;
        }

        Server server(configs);
        server.run();

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
