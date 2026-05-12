#include "server/Epoll.hpp"
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <sys/epoll.h>
#include <unistd.h>

using namespace std;

Epoll::Epoll() {
  epollFd = epoll_create(1);
  if (epollFd < 0)
    throw runtime_error("epoll_create: " + string(strerror(errno)));
}

Epoll::~Epoll() { close(epollFd); }

int Epoll::getFd() { return epollFd; }

int Epoll::getEventsFd(unsigned int idx) {
  if (idx >= EPOLL_MAX_EVENTS)
    throw runtime_error("out of bounds");
  return epollEvents[idx].data.fd;
}

int Epoll::getEventsMask(unsigned int idx) {
  if (idx >= EPOLL_MAX_EVENTS)
    throw runtime_error("out of bounds");
  return epollEvents[idx].events;
}

void Epoll::addRead(int fd) {
  struct epoll_event ev;

  memset(&ev, 0, sizeof(ev));
  ev.events = EPOLLIN;
  ev.data.fd = fd;

  if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &ev) < 0)
    throw runtime_error("epoll_ctl: " + string(strerror(errno)));
}

void Epoll::modWrite(int fd) {
  struct epoll_event ev;

  memset(&ev, 0, sizeof(ev));
  ev.events = EPOLLOUT;
  ev.data.fd = fd;

  if (epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &ev) < 0)
    throw runtime_error("epoll_ctl: " + string(strerror(errno)));
}

void Epoll::remove(int fd) {
  if (epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL) < 0)
    throw runtime_error("epoll_ctl: " + string(strerror(errno)));
}

int Epoll::wait() {
  int numEvents;

  numEvents =
      epoll_wait(epollFd, epollEvents, EPOLL_MAX_EVENTS, EPOLL_TIMEOUT_MS);
  if (numEvents < 0)
    throw runtime_error("epoll_wait: " + string(strerror(errno)));
  return numEvents;
}
