/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: igngonza <igngonza@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 11:25:32 by igngonza          #+#    #+#             */
/*   Updated: 2026/04/15 16:39:35 by fdurban-         ###   ########.fr       */
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
#include <vector>
#include <poll.h>
#include <sys/epoll.h>

#define MAX_EVENTS 64
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
	addr.sin_addr.s_addr = INADDR_ANY;
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
	int epoll_fd = epoll_create1(0);
	if (epoll_fd == -1) {
		std::cerr << "Error en epoll_create1\n";
		return 1;
	}
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = server_fd;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev);

	struct epoll_event events[MAX_EVENTS];

	while (true) {
		std::cout << "Esperando actividad...\n";

		int ready = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (ready == -1) {
			std::cerr << "Error en epoll_wait\n";
		break;
        	}

		std::cout << ready << " fd(s) listos\n";

		for (int i = 0; i < ready; i++) {
			int fd = events[i].data.fd;

			if (fd == server_fd) {
				int client_fd = accept(server_fd, NULL, NULL);
				std::cout << "Cliente conectado: fd=" << client_fd << "\n";

				struct epoll_event client_ev;
				client_ev.events = EPOLLIN;
				client_ev.data.fd = client_fd;
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_ev);
			}
			else {
				char buffer[4096];
				std::memset(buffer, 0, sizeof(buffer));
				ssize_t bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
				if (bytes > 0) {
					std::cout << "REQUEST fd=" << fd << ":\n" << buffer << "\n";
				const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 12\r\n\r\nHola Mundo!\n";
				send(fd, response, strlen(response), 0);
				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
				close(fd);
			}
			else if (bytes == 0) {
				std::cout << "Cliente fd=" << fd << " desconectado\n";
				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
				close(fd);
			}
			else {
				std::cerr << "Error en recv fd=" << fd << "\n";
				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
				close(fd);
			}
		}
	}
}
	close(epoll_fd);
	close(server_fd);
	return 0;
	std::cout << "Server starting with config: " << config_path << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
