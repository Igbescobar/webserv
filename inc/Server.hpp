#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/epoll.h>

#define PORT 8080
#define MAX_CONNECTIONS 10
#define MAX_EVENTS 10
#define RESPONSE "HTTP/1.1 200 OK\n" \
	"Content-Type: text/plain\n" \
	"Content-Length: 12\n" \
	"\n" \
	"Hello world!"

class Server {
	private:
		int server_fd;
		int epoll_fd;
		struct epoll_event events[MAX_EVENTS];

		void socket_setup();
		void socket_create();
		void socket_bind();
		void socket_listen();

		void handle_event(int fd);
		void handle_events(int n);

		void epoll_setup();
		void epoll_add(int fd);

	public:
		Server();
		~Server();

		void run();
};

#endif
