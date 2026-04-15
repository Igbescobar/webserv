/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: igngonza <igngonza@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 11:25:32 by igngonza          #+#    #+#             */
/*   Updated: 2026/04/15 16:48:16 by igngonza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Parser.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <poll.h>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#define MAX_EVENTS 64
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
  for (std::map<std::string, std::string>::const_iterator it = cgiPass.begin();
       it != cgiPass.end(); ++it) {
    std::cout << "        " << it->first << " -> " << it->second << std::endl;
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
      std::cout << "  Client Max Body Size: " << server.getClientMaxBodySize()
                << std::endl;
    }
    if (!server.getRoot().empty()) {
      std::cout << "  Root: " << server.getRoot() << std::endl;
    }
    if (!server.getIndexFiles().empty()) {
      std::cout << "  Index: ";
      const std::vector<std::string> &serverIndexes = server.getIndexFiles();
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
      if (loc.getClientMaxBodySize() != -1) {
        std::cout << "      Client Max Body Size: "
                  << loc.getClientMaxBodySize() << std::endl;
      }
      if (!loc.getUploadPath().empty()) {
        std::cout << "      Upload Path: " << loc.getUploadPath() << std::endl;
      }
      if (loc.getReturn().first != 0 || !loc.getReturn().second.empty()) {
        std::cout << "      Return: " << loc.getReturn().first << " "
                  << loc.getReturn().second << std::endl;
      }
      printLocationCgiPass(loc.getCgiPass());
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

    // 2. Setup Server
    // Server server(config);
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
      std::cout << "Error creating the socket" << std::endl;
      return 1;
    }
    std::cout << "server_fd :" << server_fd << std::endl;
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) ==
        -1) {
      std::cout << "Error in setsockot" << std::endl;
      return 1;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
      std::cout << "Error in bind, port might be already in use" << std::endl;
      return 1;
    }
    std::cout << "Bind OK en puerto 8080\n";
    if (listen(server_fd, 128) == -1) {
      std::cout << "Error server listening" << std::endl;
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
        } else {
          char buffer[4096];
          std::memset(buffer, 0, sizeof(buffer));
          ssize_t bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
          if (bytes > 0) {
            std::cout << "REQUEST fd=" << fd << ":\n" << buffer << "\n";
            const char *response =
                "HTTP/1.1 200 OK\r\nContent-Type: "
                "text/plain\r\nContent-Length: 12\r\n\r\nHola Mundo!\n";
            send(fd, response, strlen(response), 0);
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
            close(fd);
          } else if (bytes == 0) {
            std::cout << "Cliente fd=" << fd << " desconectado\n";
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
            close(fd);
          } else {
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
