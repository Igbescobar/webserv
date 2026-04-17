#include "Server.hpp"
#include <iostream>
#include <cerrno>
#include <cstring>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>

// TODO:
// close fd if error or if destructor

Server::Server() {
	socket_setup();
	epoll_setup();
	epoll_add(server_fd);
}

Server::~Server() {
}

void Server::epoll_setup() {
	epoll_fd = epoll_create(1);
	if (epoll_fd < 0)
		throw std::runtime_error("epoll_create: " + std::string(strerror(errno)));
}

void Server::socket_setup() {
	socket_create();
	socket_bind();
	socket_listen();
}

void Server::socket_create() {
	// create socket
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
		throw std::runtime_error("socket: " + std::string(strerror(errno)));

	// reuse socket address
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw std::runtime_error("setcokopt: " + std::string(strerror(errno)));

	// set socket non-blocking
	if (fcntl(server_fd, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("fcntl: " + std::string(strerror(errno)));
}

void Server::socket_bind() {
	struct sockaddr_in addr;
	memset((char *)&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(PORT);
	if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		throw std::runtime_error("bind: " + std::string(strerror(errno)));
}

void Server::socket_listen() {
	if (listen(server_fd, MAX_CONNECTIONS) < 0)
		throw std::runtime_error("listen: " + std::string(strerror(errno)));
}

void Server::run() {
	while (true) {
		int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (num_events < 0)
			throw std::runtime_error("epoll_wait: " + std::string(strerror(errno)));
		handle_events(num_events);
	}
}

void Server::epoll_add(int fd) {
	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = fd;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) < 0)
		throw std::runtime_error("epoll_ctl: " + std::string(strerror(errno)));
}

void Server::handle_events(int n) {
	for (int i = 0; i < n; i++)
		handle_event(events[i].data.fd);
}

void Server::handle_event(int fd) {
	if (fd == server_fd) {
		int new_socket;
		struct sockaddr_in addr;
		socklen_t addr_len = sizeof(addr);
		new_socket = accept(server_fd, (struct sockaddr *)&addr, &addr_len);
		if (new_socket < 0)
			throw std::runtime_error("accept: " + std::string(strerror(errno)));

		// stuff
		char buffer[1000];
		read(new_socket, buffer, 1000);
		std::cout << buffer << std::endl;
		write(new_socket, RESPONSE, sizeof(RESPONSE));
		close(new_socket);
	}
}
