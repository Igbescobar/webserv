#include "server/Server.hpp"
#include <cerrno>
#include <cstring>
#include <stdexcept>

void Server::epollCreate() {
  epollFd = ::epoll_create(1);
  if (epollFd < 0)
    throw std::runtime_error("epoll_create: " + std::string(strerror(errno)));
}

void Server::epollAddRead(int fd) {
  struct epoll_event ev;

  std::memset(&ev, 0, sizeof(ev));
  ev.events = EPOLLIN;
  ev.data.fd = fd;

  if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &ev) < 0)
    throw std::runtime_error("epoll_ctl: " + std::string(strerror(errno)));
}

void Server::epollModWrite(int fd) {
  struct epoll_event ev;

  std::memset(&ev, 0, sizeof(ev));
  ev.events = EPOLLOUT;
  ev.data.fd = fd;

  if (epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &ev) < 0)
    throw std::runtime_error("epoll_ctl: " + std::string(strerror(errno)));
}

void Server::epollRemove(int fd) {
  if (epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL) < 0)
    throw std::runtime_error("epoll_ctl: " + std::string(strerror(errno)));
}

int Server::epollWait() {
  int numEvents;

  numEvents = epoll_wait(epollFd, epollEvents, MAX_EVENTS, -1);
  if (numEvents < 0)
    throw std::runtime_error("epoll_wait: " + std::string(strerror(errno)));
  return numEvents;
}
