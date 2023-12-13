#pragma once

#include "Client.hpp"
#include "Cmd.hpp"
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
class NumReply {
public:
  static std::string needMoreParams(const Cmd &cmd) {
    std::ostringstream oss;
    oss << cmd.getAuthor().getNick() << " ";
    oss << cmd.getName() << " :Not enough parameters";
    return _Builder(461, oss.str());
  }
  static std::string alreadyRegistered(const Cmd &cmd) {
    (void)cmd;
    return _Builder(462, ":You may not reregister");
  }
  static std::string passwdMismatch(const Cmd &cmd) {
    (void)cmd;
    return _Builder(464, ":Password incorrect");
  }
  static std::string nicknameInUse(const Cmd &cmd) {
    return _Builder(433, cmd.getAuthor().getNick()
      + " "
      + cmd.getParams().front()
      + " :Nickname is already in use");
  }

private:
  static std::string _Builder(const unsigned short code, std::string content) {
    std::ostringstream oss;
    oss << ":localhost " << code << " " << content;
    return (oss.str());
  }
};
