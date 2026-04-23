#include "Server.hpp"
#include <iostream>
#include <cerrno>
#include <cstring>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>

Server::Server() {
	socket_create();
	non_blocking(server_fd);
	socket_bind();
	socket_listen();
	epoll_create();
	epoll_read(server_fd);
}

Server::~Server() {
}

void Server::non_blocking(int fd) {
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("fcntl: " + std::string(strerror(errno)));
}

void Server::run() {
	while (true) {
		//std::cout << "read_map.size: " << read_map.size() << std::endl;
		int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (num_events < 0)
			throw std::runtime_error("epoll_wait: " + std::string(strerror(errno)));
		handle_events(num_events);
	}
}

void Server::handle_events(int n) {
	for (int i = 0; i < n; i++)
		handle_event(events[i].data.fd, events[i].events);
}

void Server::handle_event(int fd, uint32_t events) {
	if (fd == server_fd)
		handle_server();
	else
		handle_client(fd, events);
}

void Server::handle_server() {
	int new_socket;
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);

	new_socket = accept(server_fd, (struct sockaddr *)&addr, &addr_len);
	if (new_socket < 0)
		throw std::runtime_error("accept: " + std::string(strerror(errno)));

	non_blocking(new_socket);
	read_map.erase(new_socket);
	epoll_read(new_socket);
	return;
}

void Server::handle_client(int fd, uint32_t events) {
	if (events & EPOLLIN)
		client_read(fd);
	else if (events & EPOLLOUT)
		client_write(fd);
}

void Server::client_read(int fd) {
	char buffer[BUF_SIZE + 1];
	int bytes_read;

	bytes_read = read(fd, buffer, BUF_SIZE);
	if (bytes_read <= 0) {
		epoll_remove(fd);
		read_map.erase(fd);
		close(fd);
		return;
	}

	buffer[bytes_read] = '\0';
	read_map[fd] += buffer;

	std::cout << "\"" << read_map[fd] << "\"";

	size_t pos = read_map[fd].find(DELIMETER);
	if (pos == std::string::npos)
		return;

	std::cout << "done!\n";

	read_map.erase(fd);

	epoll_write(fd);
}

void Server::client_write(int fd) {
	int bytes_written;

	if (write_map[fd].empty())
		write_map[fd] = RESPONSE;

	bytes_written = write(fd, write_map[fd].c_str(), write_map[fd].size());
	if (bytes_written <= 0) {
		epoll_remove(fd);
		write_map.erase(fd);
		close(fd);
	}

	write_map[fd].erase(0, bytes_written);
	if (write_map[fd].empty()) {
		epoll_remove(fd);
		write_map.erase(fd);
		close(fd);
	}
}
