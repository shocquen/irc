#pragma once

#include "Cmd.hpp"
#include "irc.hpp"
#include <string>
#include <vector>
class Client {
public:
  Client(int socket);
  Client(const Client &copy);
  ~Client();
  Client &operator=(const Client &rhs);
  bool operator==(const int &fd) const;

  /* =========================================================================
   */

  unsigned long getId() const;
  pollfd_t getPfd() const;
  bool isAuth() const;
  bool isGoodToAuth() const;
  std::string getUsername() const;
  std::string getNick() const;
  std::string getRealName() const;

  /* =========================================================================
   */

  void disconnect(std::string ctx);
  void auth();
  void validatePwd();

  /* =========================================================================
   */

  void setUsername(std::string username);
  int setNick(std::string nick);
  void setRealName(std::string realName);

  /* =========================================================================
   */

  void sendMsg(std::string msg);
  // Return 1 if the is one or more msg to treat in _buffer.
  // Else it return 0
  int appendBuffer(std::string buffer);
  // Return list of msgs and clear them form _buffer
  std::vector<std::string> bufferToMsgs();

private:
  Client();
  static unsigned long _idCount;

  bool                 _gavePwd;
  bool                 _isAuth;
  pollfd_t             _pfd;
  std::string          _buffer;
  std::string          _nick;
  std::string          _realName;
  std::string          _username;
  unsigned long        _id;
};

std::ostream &operator<<(std::ostream &stream, const Client &client);
