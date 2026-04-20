#include "Server.hpp"
#include <iostream>
#include <cerrno>
#include <cstring>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>

// TODO:
// cleanup
// ctrl-c
// error handling
// non-blocking read/write

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

void Server::epoll_create() {
	epoll_fd = ::epoll_create(1);
	if (epoll_fd < 0)
		throw std::runtime_error("epoll_create: " + std::string(strerror(errno)));
}

void Server::socket_create() {
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
		throw std::runtime_error("socket: " + std::string(strerror(errno)));

	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw std::runtime_error("setcokopt: " + std::string(strerror(errno)));
}

void Server::non_blocking(int fd) {
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
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

void Server::epoll_read(int fd) {
	struct epoll_event ev;

	std::memset(&ev, 0, sizeof(ev));
	ev.events = EPOLLIN;
	ev.data.fd = fd;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0)
		throw std::runtime_error("epoll_ctl: " + std::string(strerror(errno)));
}

void Server::epoll_remove(int fd) {
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) < 0)
		throw std::runtime_error("epoll_ctl: " + std::string(strerror(errno)));
}

void Server::epoll_write(int fd) {
	struct epoll_event ev;

	std::memset(&ev, 0, sizeof(ev));
	ev.events = EPOLLOUT;
	ev.data.fd = fd;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev) < 0)
		throw std::runtime_error("epoll_ctl: " + std::string(strerror(errno)));
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

	//non_blocking(new_socket);
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
	char buffer[BUF_SIZE];

	if (read(fd, buffer, BUF_SIZE) <= 0) {
		epoll_remove(fd);
		close(fd);
		return;
	}

	// print request
	std::cout << buffer << std::endl;
	epoll_write(fd);
}

void Server::client_write(int fd) {
	write(fd, RESPONSE, sizeof(RESPONSE));
	epoll_remove(fd);
	close(fd);
}
