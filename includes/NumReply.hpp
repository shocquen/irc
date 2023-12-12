#pragma once

#include "Client.hpp"
#include "Cmd.hpp"
#include <ostream>
#include <sstream>
#include <string>
class NumReply {
public:
  static std::string needMoreParams(const Cmd &cmd) {
    return _Builder(461, cmd, "Not enough parameters");
  }
  static std::string alreadyRegistered(const Cmd &cmd) {
    return _Builder(462, cmd, "You may not reregister");
  }
  static std::string passwdMismatch(const Cmd &cmd) {
    return _Builder(464, cmd, "Password incorrect");
  }
  // static std::string nicknameInUse(const Cmd &cmd) {
  //   return _Builder(433, cmd, "Nickname is already in use");
  // }

private:
  static std::string _Builder(const unsigned short code, const Cmd &cmd,
                              const std::string msg) {
    Client client = cmd.getAuthor();
    std::ostringstream oss;
    oss << ":localhost " << code << " " << client.getNick() << " "
        << cmd.getName() << " :" << msg;
    return (oss.str());
  }
};
