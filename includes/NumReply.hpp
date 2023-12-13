#pragma once

#include "Client.hpp"
#include "Cmd.hpp"
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
class NumReply {
public:
  static std::string wolcome(const Client &client) {
    return _Builder("001", client.getNick() + " :Welcome to ft_irc " +
                             client.getNick() + "!" + client.getUsername() +
                             "@localhost");
  }
  static std::string needMoreParams(const Cmd &cmd) {
    std::ostringstream oss;
    oss << cmd.getAuthor().getNick() << " ";
    oss << cmd.getName() << " :Not enough parameters";
    return _Builder("461", oss.str());
  }
  static std::string passwdMismatch() {
    return _Builder("464", ":Password incorrect");
  }

  static std::string nicknameInUse(const Cmd &cmd) {
    return _Builder("433", cmd.getAuthor().getNick() + " " +
                             cmd.getParams().front() +
                             " :Nickname is already in use");
  }
  static std::string noNickNameGiven(const Client &client) {
    return _Builder("431", client.getNick() + " :No nickname given");
  }
  static std::string erroneusNickname(const Cmd &cmd) {
    return _Builder("432", cmd.getAuthor().getNick() + " " +
                             cmd.getParams().front() + " :Erroneus nickname");
  }
  static std::string alreadyRegistered(const Client &client) {
    return _Builder("462", client.getNick() + " :You may not reregister");
  }
  static std::string notRegistered(const Client &client) {
    return _Builder("451", client.getNick() + " :You have not registered");
  }
  static std::string noSuchNick(const Cmd &cmd) {
    return _Builder("401", cmd.getAuthor().getNick() + " " +
                             cmd.getParams().front() +
                             " :No such nick/channel");
  }

private:
  static std::string _Builder(const std::string code, const std::string content) {
    std::ostringstream oss;
    oss << ":localhost " << code << " " << content;
    return (oss.str());
  }
};
