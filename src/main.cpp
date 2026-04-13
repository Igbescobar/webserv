/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: igngonza <igngonza@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 11:25:32 by igngonza          #+#    #+#             */
/*   Updated: 2026/04/13 16:59:17 by fdurban-         ###   ########.fr       */
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
	addr.sin_addr.s_addr = inet_addr("10.13.10.3");
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
	  std::vector<struct pollfd> fds;

    // Añadir server_fd como primer elemento
    struct pollfd server_pfd;
    server_pfd.fd = server_fd;
    server_pfd.events = POLLIN;   // queremos saber cuando haya datos/conexiones
    server_pfd.revents = 0;
    fds.push_back(server_pfd);

    while (true) {
        std::cout << "Esperando actividad en " << fds.size() << " fds...\n";

        // Poll NO reconstruye — el vector persiste entre iteraciones
        int ready = poll(fds.data(), fds.size(), -1);
        if (ready == -1) {
            std::cerr << "Error en poll\n";
            break;
        }

        std::cout << ready << " fd(s) listos\n";

        // Iterar sobre todos los fds para ver cuáles tienen actividad
        for (size_t i = 0; i < fds.size(); i++) {

            // revents — lo que OCURRIÓ (el kernel lo rellena)
            // events  — lo que PEDISTE (tú lo rellenas, el kernel no lo toca)
            if (!(fds[i].revents & POLLIN))
                continue;

            if (fds[i].fd == server_fd) {
                // Conexión nueva
                int client_fd = accept(server_fd, NULL, NULL);
                std::cout << "Cliente conectado: fd=" << client_fd << "\n";

                struct pollfd client_pfd;
                client_pfd.fd = client_fd;
                client_pfd.events = POLLIN;
		std::cout<<"Valor de POLLIN: "<<POLLIN<<std::endl;
                client_pfd.revents = 0;
                fds.push_back(client_pfd);  // añadir al vector — persiste
            } else {
                // Datos de un cliente existente
                char buffer[4096];
                std::memset(buffer, 0, sizeof(buffer));
                ssize_t bytes = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);

                if (bytes > 0) {
                    std::cout << "REQUEST fd=" << fds[i].fd << ":\n" << buffer << "\n";
                    const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 12\r\n\r\nHola Mundo!\n";
                    send(fds[i].fd, response, strlen(response), 0);
                    close(fds[i].fd);
                    fds.erase(fds.begin() + i);  // eliminar del vector
                    i--;  // ajustar índice tras erase
                } else if (bytes == 0) {
                    std::cout << "Cliente fd=" << fds[i].fd << " desconectado\n";
                    close(fds[i].fd);
                    fds.erase(fds.begin() + i);
                    i--;
                } else {
                    std::cerr << "Error en recv fd=" << fds[i].fd << "\n";
                    close(fds[i].fd);
                    fds.erase(fds.begin() + i);
                    i--;
                }
            }
        }
    }

    close(server_fd);
    return 0;
    std::cout << "Server starting with config: " << config_path << std::endl;

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
