#pragma once

#include <sys/epoll.h>
#include <set> //temporary

#define EPOLL_TIMEOUT_MS 100
#define EPOLL_MAX_EVENTS 10

class Epoll {
private:
  int epollFd;
  struct epoll_event epollEvents[EPOLL_MAX_EVENTS];
  std::set<int> registeredFds;
public:
  Epoll();
  ~Epoll();

  int getFd();
  int getEventsFd(unsigned int idx);
  int getEventsMask(unsigned int idx);

  void addRead(int fd);
  void addWrite(int fd);
  void modWrite(int fd);
  void remove(int fd);
  int wait();
  //temporary
  void printRegistered() const;
};
