#include "server/Client.hpp"
#include "parser/config/ServerConfig.hpp"
#include "server/Socket.hpp"
#include <unistd.h>

Client::Client(int clientSocket, Epoll &epoll, ServerConfig &serverConfig)
    : clientFd(clientSocket), epoll(epoll), serverConfig(serverConfig) {
  Socket::setNonBlocking(clientSocket);
  epoll.addRead(clientSocket);
}

Client::~Client() { close(clientFd); }

int Client::handle(uint32_t eventsMask) {
  if (eventsMask & EPOLLIN)
    return read(clientFd);
  else if (eventsMask & EPOLLOUT)
    return write(clientFd);
  return CLIENT_DELETE;
}

int Client::read(int clientFd) {
  char buffer[BUF_SIZE + 1];
  int bytesRead;

  bytesRead = ::read(clientFd, buffer, BUF_SIZE);
  if (bytesRead <= 0) {
    epoll.remove(clientFd);
    return CLIENT_DELETE;
  }

  buffer[bytesRead] = '\0';
}

int Client::write(int clientFd) {}

// void Server::clientRead(int fd) {
//   char buffer[BUF_SIZE + 1];
//   int bytes_read;
//
//   bytes_read = read(fd, buffer, BUF_SIZE);
//   if (bytes_read <= 0) {
//     epoll.remove(fd);
//     requestMap.erase(fd);
//     close(fd);
//     return;
//   }
//
//   buffer[bytes_read] = '\0';
//   requestMap[fd].append(buffer);
//
//   switch (requestMap[fd].getState()) {
//   case INCOMPLETE:
//     return;
//   case COMPLETE:
//     responseMap[fd] =
//         HttpResponse(requestMap[fd].getServerConfig(), requestMap[fd]);
//     break;
//   case ERROR:
//     responseMap[fd] = HttpResponse(requestMap[fd].getServerConfig(),
//                                    requestMap[fd].getErrorCode());
//     break;
//   default:
//     throw std::runtime_error("undefined t_state value");
//   }
//
//   requestMap.erase(fd);
//
//   epoll.modWrite(fd);
// }
//
// void Server::clientWrite(int fd) {
//   int bytes_written = write(fd, responseMap[fd].getResponse().c_str(),
//                             responseMap[fd].getResponse().size());
//
//   if (bytes_written <= 0) {
//     epoll.remove(fd);
//     responseMap.erase(fd);
//     close(fd);
//   }
//
//   responseMap[fd].erase(bytes_written);
//
//   if (responseMap[fd].empty()) {
//     epoll.remove(fd);
//     responseMap.erase(fd);
//     close(fd);
//   }
// }
