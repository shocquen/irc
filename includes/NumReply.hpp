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
  static std::string passwdMismatch() {
    return _Builder(464, ":Password incorrect");
  }

  static std::string nicknameInUse(const Cmd &cmd) {
    return _Builder(433, cmd.getAuthor().getNick() + " " +
                             cmd.getParams().front() +
                             " :Nickname is already in use");
  }
  static std::string noNickNameGiven(const Client &client) {
    return _Builder(431, client.getNick() + " :No nickname given");
  }
  static std::string erroneusNickname(const Cmd &cmd) {
    return _Builder(432, cmd.getAuthor().getNick() + " " +
                             cmd.getParams().front() + " :Erroneus nickname");
  }
  static std::string alreadyRegistered(const Client &client) {
    return _Builder(462, client.getNick() + " :You may not reregister");
  }

private:
  static std::string _Builder(const unsigned short code, std::string content) {
    std::ostringstream oss;
    oss << ":localhost " << code << " " << content;
    return (oss.str());
  }
};
