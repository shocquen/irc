#pragma once

#include "irc.hpp"
#include <string>
#include <vector>
class Client {
public:
  Client(int socket);
  Client(const Client &copy);
  ~Client();
  Client &operator=(const Client &rhs);
  bool operator==(const int &fd);

  int getId() const;
  pollfd_t getPfd() const;

  void disconnect();
  void sendMsg(std::string msg);
  int appendBuffer(std::string buffer);
  std::vector<std::string> bufferToMsgs(); // Return list of msgs and clear them form _buffer

private:
  Client();
  static unsigned long _idCount;
  unsigned long _id;
  pollfd_t _pfd;
  std::string _buffer;
};
