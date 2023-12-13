#include "Client.hpp"
#include "Server.hpp"
#include <cerrno>
#include <cstring>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>

unsigned long Client::_idCount = 0;

Client::Client(int socket) {
  _id = _idCount++;
  _pfd.fd = socket;
  _pfd.events = POLLIN | POLLOUT;
  _isAuth = false;
}

Client::Client(const Client &copy) { *this = copy; }

Client &Client::operator=(const Client &rhs) {
  _id = rhs._id;
  _pfd = rhs._pfd;
  _isAuth = rhs._isAuth;
  return (*this);
}

Client::~Client() {}

/* ========================================================================= */

bool Client::operator==(const int &fd) { return _pfd.fd == fd; }

/* ========================================================================= */

int Client::getId() const { return (_id); }
pollfd_t Client::getPfd() const { return (_pfd); }
bool Client::isAuth() const { return _isAuth; }
bool Client::gavePwd() const { return _validatePwd; }
bool Client::isGoodToAuth() const {
  if (_username.empty() || _nick.empty() || _realName.empty())
    return (false);
  return (true);
}
std::string Client::getUsername() const { return _username; }
std::string Client::getNick() const {
  return (_nick.empty() ? _username : _nick);
}
std::string Client::getRealName() const { return _realName; }

/* ========================================================================= */

void Client::auth() { _isAuth = true; }

void Client::validatePwd() { _validatePwd = true; }

void Client::setUsername(std::string username) {
  if (_isAuth) {
    sendMsg("TODO ERR userame");
    return;
  }
  if (username.empty()) {
    sendMsg("TODO ERR username empty");
    return;
  }
  _username = username;
}

void Client::setNick(std::string nick) {
  if (nick.find(" ") != nick.size()) {
    sendMsg("TODO ERR NICK");
    return;
  }
  if (nick.empty()) {
    sendMsg("TODO ERR NICK empty");
    return;
  }
  _nick = nick;
}

void Client::setRealName(std::string realName) { _realName = realName; }

/* ------------------------------------------------------------------------- */

void Client::disconnect() {
  close(_pfd.fd);
  _pfd.fd = -1;
  std::cout << "client[" << _id << "] is disconnected" << std::endl;
}

void Client::sendMsg(std::string msg) {
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
