#include "Server.hpp"
#include <cerrno>
#include <cstring>
#define BUFFER_SIZE 2024

Server::ServerException::~ServerException() throw() { _msg.clear(); };
Server::ServerException::ServerException(std::string const msg) : _msg(msg) {}
Server::ServerException::ServerException(std::string const msg, int errnoValue)
    : _msg(msg) {
  _msg += ": " + std::string(strerror(errnoValue));
}

const char *Server::ServerException::what() const throw() {
  return _msg.c_str();
}

/* ========================================================================= */

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

void Server::run() {
  ssize_t s;
  char buf[BUFFER_SIZE];
  int newSocket;
  struct sockaddr_in clientAddress;
  socklen_t addrlen = sizeof(clientAddress);
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
        if ((newSocket = accept(_fd, (struct sockaddr *)&clientAddress,
                                &addrlen)) < 0) {
          throw ServerException("accept", errno);
        } else {
          _clients.push_back(Client(newSocket));
        }
      } else {
        bzero(buf, BUFFER_SIZE);
        s = recv(pfds[i].fd, buf, BUFFER_SIZE, MSG_DONTWAIT);
        if (s == -1) {
          throw ServerException("recv", errno);
        }
        std::vector<Client>::iterator client = _clients.end();
        for (std::vector<Client>::iterator it = _clients.begin();
             it != _clients.end(); it++) {
          if (it->getPfd().fd == pfds[i].fd) {
            client = it;
            break;
          }
        }
        if (client == _clients.end()) {
          std::cerr << "No client found" << std::endl;
        }
        if (s == 0) {
          client->disconnect();
          printf("\tEreasing fd %d\n", pfds[i].fd);
          _clients.erase(client);
        } else {
          printf("\tread from %u, %zd bytes: %.*s\n", client->getId(), s,
                 (int)s, buf);
        }
      }
    }
  }
}

void Server::stop() { close(_fd); }
