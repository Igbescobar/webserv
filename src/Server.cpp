#include "../inc/Server.hpp"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sstream>

Server::Server(const std::vector<ServerConfig> &configs)
    : epoll_fd(-1), configs(configs) {
}

Server::~Server() {
    if (this->epoll_fd != -1) {
        close(this->epoll_fd);
    }
    for (size_t i = 0; i < this->server_fds.size(); ++i) {
        if (this->server_fds[i] != -1) {
            close(this->server_fds[i]);
        }
    }
}

bool Server::isAddressBound(const std::string &ip, int port) const {
    std::stringstream ss;
    ss << ip << ":" << port;
    std::string target = ss.str();
    return std::find(this->bound_addresses.begin(), this->bound_addresses.end(),
                     target) != this->bound_addresses.end();
}

void Server::setupSocket() {
    const std::vector<std::string> &listens = this->configs[0].getListen();

    if (listens.empty()) {
        throw std::runtime_error("No listen ports specified in config.");
    }

    for (size_t c = 0; c < this->configs.size(); ++c) {
        const std::vector<std::string> &listens = this->configs[c].getListen();

        for (size_t i = 0; i < listens.size(); ++i) {
            std::string listen_str = listens[i];
            std::string ip = "0.0.0.0";
            std::string port_str = listen_str;

            size_t colon_pos = listen_str.find(':');
            if (colon_pos != std::string::npos) {
                ip = listen_str.substr(0, colon_pos);
                port_str = listen_str.substr(colon_pos + 1);
            }

            if (ip == "localhost") {
                ip = "127.0.0.1";
            } else if (ip == "*") {
                ip = "0.0.0.0";
            }

            int port = std::atoi(port_str.c_str());
            if (port == 0)
                port = 8080;

            if (this->isAddressBound(ip, port)) {
                continue;
            }

            int fd = socket(AF_INET, SOCK_STREAM, 0);
            if (fd == -1) {
                throw std::runtime_error("Error creating the socket");
            }

            int opt = 1;
            if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) ==
                -1) {
                close(fd);
                throw std::runtime_error("Error in setsockopt");
            }

            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(port);
            addr.sin_addr.s_addr = inet_addr(ip.c_str());

            if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
                close(fd);
                std::cerr << "Address " << ip << ":" << port
                          << " failed to bind.\n";
                throw std::runtime_error(
                    "Error in bind, port might be already in use");
            }

            if (listen(fd, 128) == -1) {
                close(fd);
                throw std::runtime_error("Error server listening");
            }

            std::cout << "Bind OK on port " << port << " (fd: " << fd << ")\n";
            this->server_fds.push_back(fd);

            std::stringstream ss;
            ss << ip << ":" << port;
            this->bound_addresses.push_back(ss.str());
        }
    }
    if (this->server_fds.empty()) {
        throw std::runtime_error("No valid ports parsed to listen on.");
    }
}

void Server::setupEpoll() {
    this->epoll_fd = epoll_create1(0);
    if (this->epoll_fd == -1) {
        throw std::runtime_error("Error in epoll_create1");
    }

    for (size_t i = 0; i < this->server_fds.size(); ++i) {
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = this->server_fds[i];

        if (epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, this->server_fds[i],
                      &ev) == -1) {
            throw std::runtime_error("Error in epoll_ctl ADD server_fd");
        }
    }
}

bool Server::isServerSocket(int fd) const {
    return std::find(this->server_fds.begin(), this->server_fds.end(), fd) !=
           this->server_fds.end();
}

void Server::handleNewConnection(int server_fd) {
    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd == -1) {
        std::cerr << "Error in accept\n";
        return;
    }
    std::cout << "Client connected: fd=" << client_fd << "\n";

    struct epoll_event client_ev;
    client_ev.events = EPOLLIN;
    client_ev.data.fd = client_fd;

    if (epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, client_fd, &client_ev) == -1) {
        std::cerr << "Error adding client to epoll\n";
        close(client_fd);
    }
}

void Server::handleClientRequest(int client_fd) {
    char buffer[4096];
    std::memset(buffer, 0, sizeof(buffer));

    ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes > 0) {
        std::cout << "REQUEST fd=" << client_fd << ":\n" << buffer << "\n";
        const char *response =
            "HTTP/1.1 200 OK\r\nContent-Type: "
            "text/plain\r\nContent-Length: 12\r\n\r\nHola Mundo!\n";
        send(client_fd, response, strlen(response), 0);

        epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
        close(client_fd);
    } else if (bytes == 0) {
        std::cout << "Client fd=" << client_fd << " disconnected\n";
        epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
        close(client_fd);
    } else {
        std::cerr << "Error in recv fd=" << client_fd << "\n";
        epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
        close(client_fd);
    }
}

void Server::run() {
    std::cout << "Starting Server with parsed configuration..." << std::endl;
    this->setupSocket();
    this->setupEpoll();

    struct epoll_event events[MAX_EVENTS];

    while (true) {
        std::cout << "Waiting for activity...\n";

        int ready = epoll_wait(this->epoll_fd, events, MAX_EVENTS, -1);
        if (ready == -1) {
            std::cerr << "Error in epoll_wait\n";
            break;
        }

        std::cout << ready << " fd(s) ready\n";

        for (int i = 0; i < ready; i++) {
            int active_fd = events[i].data.fd;

            if (this->isServerSocket(active_fd)) {
                this->handleNewConnection(active_fd);
            } else {
                this->handleClientRequest(active_fd);
            }
        }
    }
}
