#pragma once

#include <arpa/inet.h>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#include "Parser.hpp"
#include "ServerConfig.hpp"

#define MAX_EVENTS 64

class Server {
  private:
    std::vector<int> server_fds;
    std::vector<std::string> bound_addresses;
    int epoll_fd;
    std::vector<ServerConfig> configs;

    void setupSocket();
    void setupEpoll();
    void handleNewConnection(int server_fd);
    void handleClientRequest(int client_fd);
    bool isServerSocket(int fd) const;
    bool isAddressBound(const std::string &ip, int port) const;

  public:
    Server(const std::vector<ServerConfig> &configs);
    ~Server();

    void run();
};
