#include "Server.hpp"
#include <sys/socket.h>
#include <stdexcept>
#include <cstring>
#include <cerrno>
#include <netinet/in.h>

void Server::socket_create() {
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
		throw std::runtime_error("socket: " + std::string(strerror(errno)));

	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw std::runtime_error("setcokopt: " + std::string(strerror(errno)));
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
