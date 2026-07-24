#include "utils.hpp"
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <string>

void setNonBlocking(int fd) {
  if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
    throw std::runtime_error("fcntl: " + std::string(strerror(errno)));
}

void setCloseOnExec(int fd) {
  if (fcntl(fd, F_SETFL, FD_CLOEXEC) < 0)
    throw std::runtime_error("fcntl: " + std::string(strerror(errno)));
}
