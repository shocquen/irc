#include "Client.hpp"
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>

unsigned long Client::_idCount = 0;

Client::Client(int socket) {
  _id = _idCount++;
  _pfd.fd = socket;
  _pfd.events = POLLIN | POLLOUT;
}

Client::Client(const Client &copy) {
  *this = copy;
}

Client & Client::operator=(const Client &rhs) {
  _id = rhs._id;
  _pfd = rhs._pfd;
  return (*this);
}

Client::~Client() {}

int Client::getId() const {
  return (_id);
}

pollfd_t Client::getPfd() const {
  return (_pfd);
}

void Client::disconnect() {
  close(_pfd.fd);
  _pfd.fd = -1;
  std::cout << "Client " << _id << " is disconnected" << std::endl;
}

void Client::sendMsg(std::string msg) {
  msg += "\r\n";
  send(_pfd.fd, msg.c_str(), msg.size(), MSG_DONTWAIT);
}
