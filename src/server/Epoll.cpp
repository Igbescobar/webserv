#include "server/Epoll.hpp"
#include "utils.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <sys/epoll.h>
#include <unistd.h>

Epoll::Epoll() {
  epollFd = epoll_create(1);
  if (epollFd < 0)
    throw std::runtime_error("epoll_create: " + std::string(strerror(errno)));
  setCloseOnExec(epollFd);
}

Epoll::~Epoll() { close(epollFd); }

int Epoll::getFd() { return epollFd; }

int Epoll::getEventsFd(unsigned int idx) {
  if (idx >= EPOLL_MAX_EVENTS)
    throw std::runtime_error("out of bounds");
  return epollEvents[idx].data.fd;
}

int Epoll::getEventsMask(unsigned int idx) {
  if (idx >= EPOLL_MAX_EVENTS)
    throw std::runtime_error("out of bounds");
  return epollEvents[idx].events;
}

void Epoll::addRead(int fd) {
  struct epoll_event ev;

  std::memset(&ev, 0, sizeof(ev));
  ev.events = EPOLLIN;
  ev.data.fd = fd;

  if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &ev) < 0)
    throw std::runtime_error("epoll_ctl: " + std::string(strerror(errno)));
  registeredFds.insert(fd);
}

void Epoll::modWrite(int fd) {
  struct epoll_event ev;

  std::memset(&ev, 0, sizeof(ev));
  ev.events = EPOLLOUT;
  ev.data.fd = fd;

  if (epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &ev) < 0)
    throw std::runtime_error("epoll_ctl: " + std::string(strerror(errno)));
}

void Epoll::remove(int fd) {
  if (epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL) < 0)
    throw std::runtime_error("epoll_ctl: " + std::string(strerror(errno)));
  registeredFds.erase(fd);
}

int Epoll::wait() {
  int numEvents;

  numEvents =
      epoll_wait(epollFd, epollEvents, EPOLL_MAX_EVENTS, EPOLL_TIMEOUT_MS);
  if (numEvents < 0 && errno != EINTR)
    throw std::runtime_error("epoll_wait: " + std::string(strerror(errno)));
  return numEvents;
}

void Epoll::printRegistered() const {
    std::cerr << "[EPOLL] registered fds: ";
    for (std::set<int>::const_iterator it = registeredFds.begin();
         it != registeredFds.end(); ++it)
        std::cerr << *it << " ";
    std::cerr << "\n";
}
