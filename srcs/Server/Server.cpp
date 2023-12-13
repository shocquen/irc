#include "Server.hpp"
#include "Cmd.hpp"
#include "Utils.hpp"
#include <algorithm>
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#define BUFFER_SIZE 5

Server::Server(std::string pwd, unsigned short port) {
  _pwd = pwd;
  _port = port;

  _fd = socket(AF_INET, SOCK_STREAM, 0);

  if (_fd < 0) {
    throw ServerException("socket failed", errno);
  }

  int opt = 1;
  struct sockaddr_in serverAddress;
  if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt))) {
    close(_fd);
    throw ServerException("setsocketopt", errno);
  }
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  serverAddress.sin_port = htons(_port);

  if (bind(_fd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
    close(_fd);
    throw ServerException("bind failed", errno);
  }
  if (listen(_fd, 3) < 0) {
    close(_fd);
    throw ServerException("listen", errno);
  }
  std::cout << "New server listenning on port " << port << std::endl;
}

Server::Server(const Server &copy) { *this = copy; }

Server &Server::operator=(const Server &rhs) {
  _fd = rhs._fd;
  _pwd = rhs._pwd;
  _port = rhs._port;
  _clients = rhs._clients;
  // _channels = rhs._channels;
  return (*this);
}

Server::~Server(){};

/* ========================================================================= */

void Server::_acceptNewClient() {
  int newSocket;
  struct sockaddr_in clientAddress;
  socklen_t addrlen = sizeof(clientAddress);

  if ((newSocket = accept(_fd, (struct sockaddr *)&clientAddress, &addrlen)) <
      0) {
    throw ServerException("accept", errno);
  } else {
    _clients.push_back(Client(newSocket));
    std::cout << "New client" << std::endl;
  }
}

int Server::_readFromClient(const _ClientIterator &client) {
  ssize_t s;
  char buf[BUFFER_SIZE];

  bzero(buf, BUFFER_SIZE);
  s = recv(client->getPfd().fd, buf, BUFFER_SIZE - 1, MSG_DONTWAIT);
  if (s == -1) {
    throw ServerException("recv", errno);
  }

  if (s == 0) {
    client->disconnect();
    _clients.erase(client);
  } else {
    if (client->appendBuffer(std::string(buf)))
      return (1);
  }
  return (0);
}

void Server::run() {
  while (true) {
    nfds_t nfds = _clients.size() + 1;
    struct pollfd pfds[nfds];

    pfds[0].fd = _fd;
    pfds[0].events = POLLIN;
    for (nfds_t i = 1; i < nfds; i++) {
      pfds[i] = _clients[i - 1].getPfd();
    }

    if (poll(pfds, nfds, -1) == -1) {
      throw ServerException("poll", errno);
    }
    for (nfds_t i = 0; i < nfds; i++) {
      if (!(pfds[i].revents & POLLIN))
        continue;
      if (pfds[i].fd == _fd) {
        _acceptNewClient();
        continue;
      }

      _ClientIterator client =
          std::find(_clients.begin(), _clients.end(), pfds[i].fd);
      if (client == _clients.end()) {
        throw ServerException("No client found with fd: " +
                              to_string(pfds[i].fd));
      }

      if (_readFromClient(client) == 0) // If now complete msg
        continue;

      std::vector<std::string> msgs = client->bufferToMsgs();
      for (size_t i = 0; i < msgs.size(); i++) {
        std::cout << "client[" << client->getId() << "]: " << msgs[i]
                  << std::endl;

        Cmd cmd(*client, msgs[i]);
        if (_cmdHandlers.find(cmd.getName()) == _cmdHandlers.end())
          continue;
        _cmdFuncPtr f = _cmdHandlers.at(cmd.getName());
        (this->*f)(cmd);
      }
    }
  }
}

void Server::stop() { close(_fd); }
