#pragma once

#include "irc.hpp"
#include <string>
class Client {
public:
  Client(int socket);
  Client(const Client &copy);
  ~Client();
  Client &operator=(const Client &rhs);

  int getId() const;
  pollfd_t getPfd() const;

  void disconnect();
  void sendMsg(std::string msg);

private:
  Client();
  static unsigned long _idCount;
  unsigned long _id;
  pollfd_t _pfd;
};
