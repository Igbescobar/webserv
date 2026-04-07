/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: igngonza <igngonza@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 11:25:32 by igngonza          #+#    #+#             */
/*   Updated: 2026/04/07 17:22:03 by fdurban-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char **argv) {
  if (argc > 2) {
    std::cerr << "Usage: " << argv[0] << " [configuration_file]" << std::endl;
    return 1;
  }

  const char *config_path = (argc == 2) ? argv[1] : "config/default.conf";

  try {
    // 1. Parse Configuration
    // Config config(config_path);

    // 2. Setup Server
    // Server server(config);
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(!server_fd)
	{
		std::cout<<"Error creating the socket"<<std::endl;
		return 1;
	}
	std::cout<<"server_fd :"<<server_fd<<std::endl;
	int opt = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	struct sockaddr_in addr;
	addr.sin_family =  AF_INET;
	addr.sin_port = htons(8080);
	addr.sin_addr.s_addr = inet_addr("10.13.12.1");
	bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
	std::cout<<"bind done: "<<addr.sin_family<<std::endl;
	listen(server_fd, 128);
	int client_fd = accept(server_fd, NULL, NULL);
	std::cout<<client_fd<<std::endl;
	/*if(client_fd)
		std::cout<<client_fd<<std::endl;
	char buffer[1024];
	while(1)
	{
		ssize_t n = read(client_fd, buffer, 1023);
		buffer[n] = '\0';
		std::cout<<buffer<<std::endl;
	}*/
    // 3. Run Server
    // server.run();

    std::cout << "Server starting with config: " << config_path << std::endl;

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
