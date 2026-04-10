/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: igngonza <igngonza@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 11:25:32 by igngonza          #+#    #+#             */
/*   Updated: 2026/04/10 13:49:38 by fdurban-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>

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
	if(server_fd == -1)
	{
		std::cout<<"Error creating the socket"<<std::endl;
		return 1;
	}
	std::cout<<"server_fd :"<<server_fd<<std::endl;
	int opt = 1;
	if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		std::cout<<"Error in setsockot"<<std::endl;
		return 1;
	}
	struct sockaddr_in addr;
	addr.sin_family =  AF_INET;
	addr.sin_port = htons(8080);
	addr.sin_addr.s_addr = inet_addr("10.13.10.2");
	if(bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
	{
		std::cout<<"Error in bind, port might be already in use"<<std::endl;
		return 1;
	}
	std::cout << "Bind OK en puerto 8080\n";
	if(listen(server_fd, 128) == -1)
	{
		std::cout<<"Error server listening"<<std::endl;
		return -1;
	}
	std::cout << "Esperando conexiones...\n";
	while(true)
	{
		int client_fd = accept(server_fd, NULL, NULL);   // bloqueante → espera hasta que llegue un cliente
		std::cout << "¡Cliente conectado! fd: " << client_fd << "\n";
		int flags = fcntl(client_fd, F_GETFL, 0);
		// aquí client_fd ya existe, la conexión TCP está establecida
		fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
		char buffer[4096];
		std::memset(buffer, 0, sizeof(buffer));
		ssize_t bytes_received;
		bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
		if (bytes_received > 0)
		{
			std::cout << "\n--- DATOS RECIBIDOS ---\n";
			std::cout << buffer;
			std::cout << "\n-----------------------\n";
			const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 12\r\n\r\nHola Mundo!\n";
			send(client_fd, response, std::strlen(response), 0);
		}
		else if (bytes_received == 0)
		{
			std::cout << "El cliente cerró la conexión antes de enviar nada.\n";
		}
		else
		{
			std::cerr << "Error en recv\n";
		}
		close(client_fd);
	}
	//close(client_fd);
	close(server_fd);
	/*
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
