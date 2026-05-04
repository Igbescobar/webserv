#include "Server.hpp"
#include <stdexcept>
#include <cerrno>
#include <cstring>

void Server::epoll_create() {
	epoll_fd = ::epoll_create(1);
	if (epoll_fd < 0)
		throw std::runtime_error("epoll_create: " + std::string(strerror(errno)));
}

void Server::epoll_read(int fd) {
	struct epoll_event ev;

	std::memset(&ev, 0, sizeof(ev));
	ev.events = EPOLLIN;
	ev.data.fd = fd;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0)
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

void Server::epoll_remove(int fd) {
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) < 0)
		throw std::runtime_error("epoll_ctl: " + std::string(strerror(errno)));
}
