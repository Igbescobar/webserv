#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <string>
#include <sys/epoll.h>

#define PORT 8080
#define MAX_CONNECTIONS 10
#define MAX_EVENTS 10
#define RESPONSE                                                               \
  "HTTP/1.1 200 OK\n"                                                          \
  "Content-Type: text/plain\n"                                                 \
  "Content-Length: 13\n"                                                       \
  "\n"                                                                         \
  "Hello world!\n"
#define BUF_SIZE 4096
#define DELIMETER "\r\n\r\n"

// TODO:
// cleanup
// ctrl-c
// error handling
// non-blocking read/write
// control never-ending request
// control timeout request
// do not stop if max open files reached

class Server {
private:
  int server_fd;
  int epoll_fd;
  struct epoll_event events[MAX_EVENTS];
  std::map<int, std::string> read_map;
  std::map<int, std::string> write_map;

  void socket_create();
  void socket_bind();
  void socket_listen();

  void handle_event(int fd, uint32_t events);
  void handle_events(int n);

  void handle_server();
  void handle_client(int fd, uint32_t events);

  void client_read(int fd);
  void client_write(int fd);

  void epoll_create();
  void epoll_read(int fd);
  void epoll_write(int fd);
  void epoll_remove(int fd);

  void non_blocking(int fd);

public:
  Server();
  ~Server();

  void run();
};

#endif
