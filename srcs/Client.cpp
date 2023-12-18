#include "Client.hpp"
#include "NumReply.hpp"
#include "Server.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

unsigned long Client::_idCount = 0;

Client::Client(int socket) {
  _id = _idCount++;
  _pfd.fd = socket;
  _pfd.events = POLLIN | POLLOUT;

  _isRegistered = false;
  _GoodToRegister = false;
}

Client::Client(const Client &copy) { *this = copy; }

Client &Client::operator=(const Client &rhs) {
  _id = rhs._id;
  _pfd = rhs._pfd;
  _isRegistered = rhs._isRegistered;
  _GoodToRegister = rhs._GoodToRegister;
  _username = rhs._username;
  _nick = rhs._nick;
  _realName = rhs._realName;
  return (*this);
}

Client::~Client() {}

/* ========================================================================= */

bool Client::operator==(const int &fd) const { return _pfd.fd == fd; }
bool Client::operator==(const Client &rhs) const { return _id == rhs._id; }
bool Client::operator==(const std::string nick) const { return _nick == nick; }
bool Client::operator==(const std::vector<Client*>::const_iterator &rhs) const { return _id == (*rhs)->_id; }

/* ========================================================================= */

unsigned long Client::getId() const { return (_id); }
pollfd_t Client::getPfd() const { return (_pfd); }
bool Client::isRegistered() const { return _isRegistered; }
bool Client::checkRegistered() const { return (isRegistered()); }
bool Client::isGoodToRegister() const { return _GoodToRegister; }
std::string Client::getUsername() const { return _username; }
std::string Client::getNick() const { return (_nick.empty() ? "*" : _nick); }
std::string Client::getRealName() const { return _realName; }

/* ========================================================================= */

void Client::setRegistered() {
  _isRegistered = true;
  std::cout << *this << " registered" << std::endl;
  sendMsg(NumReply::wolcome(*this));
}

void Client::setGoodToRegister() { _GoodToRegister = true; }

int Client::setUsername(std::string username) {
  if (username.empty())
    return 1;
  if (username.find(" ") != username.npos)
    return 1;

  _username = username;
  std::cout << *this << " set his username to " << username << std::endl;
  return 0;
}

int Client::setNick(std::string nick) {
  if (nick.empty())
    return 1;
  if (nick.find(" ") != nick.npos)
    return 1;
  if (nick.find("#") != nick.npos)
    return 1;
  if (nick.find(":") != nick.npos)
    return 1;

  std::cout << *this << " changed his nickname to " << nick << std::endl;
  _nick = nick;
  return 0;
}

void Client::setRealName(std::string realName) {
  _realName = realName;
  std::cout << *this << " set his real name to " << realName << std::endl;
}

/* ------------------------------------------------------------------------- */

void Client::disconnect(std::string ctx) {
  close(_pfd.fd);
  _pfd.fd = -1;
  std::cout << *this << " is disconnected";
  if (ctx.empty() == false)
    std::cout << ": " << ctx;
  std::cout << std::endl;
}

void Client::sendMsg(std::string msg) const {
  std::cout << "-> " << *this << ": " << msg << std::endl;
  msg += "\r\n";
  send(_pfd.fd, msg.c_str(), msg.size(), MSG_DONTWAIT);
}

int Client::appendBuffer(std::string buffer) {
  _buffer += buffer;
  if (_buffer.find("\r\n") != std::string::npos)
    return (1);
  return (0);
}

std::vector<std::string> Client::bufferToMsgs() {
  std::vector<std::string> msgs;

  std::string::size_type pos;
  while ((pos = _buffer.find("\r\n")) != std::string::npos) {
    msgs.push_back(_buffer.substr(0, pos));
    _buffer = _buffer.substr(pos + 2, _buffer.size());
  }
  return (msgs);
}

/* ========================================================================= */

std::ostream &operator<<(std::ostream &stream, const Client &client) {
  if (client.getNick() != "*")
    stream << client.getNick();
  else if (client.getUsername().empty() == false)
    stream << client.getUsername();
  else
    stream << "client[" << client.getId() << "]";
  return stream;
}
