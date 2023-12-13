#include "Cmd.hpp"
#include "NumReply.hpp"
#include "Server.hpp"
#include <iostream>
#include <sstream>

std::map<std::string, Server::_cmdFuncPtr> Server::initCmdHandlers() {
  std::map<std::string, Server::_cmdFuncPtr> m;
  m["PASS"] = &Server::_handlePASS;
  m["NICK"] = &Server::_handleNICK;
  m["USER"] = &Server::_handleUSER;
  m["PING"] = &Server::_handlePING;
  m["PRIVMSG"] = &Server::_handlePRIVMSG;
  return m;
}

const std::map<std::string, Server::_cmdFuncPtr> Server::_cmdHandlers =
    Server::initCmdHandlers();

/* ========================================================================= */

void Server::_handlePASS(const Cmd &cmd) {
  Client &client = cmd.getAuthor();
  if (cmd.getParams().empty()) {
    client.sendMsg(NumReply::needMoreParams(cmd));
    return;
  }
  if (client.isRegistered()) {
    client.sendMsg(NumReply::alreadyRegistered(cmd.getAuthor()));
    return;
  }
  if (cmd.getParams().front() != _pwd) {
    client.sendMsg(NumReply::passwdMismatch());
    return;
  }
  client.setGoodToRegister();
  std::cout << "client[" << client.getId() << "] validate PASS" << std::endl;
}

void Server::_handleNICK(const Cmd &cmd) {
  Client &client = cmd.getAuthor();
  if (client.isGoodToRegister() == false) {
    client.sendMsg("ERROR :tried to NICK before PASS");
    _disconnectClient(client, "tried to NICK before PASS");
    return;
  }

  if (cmd.getParams().empty()) {
    client.sendMsg(NumReply::noNickNameGiven(client));
    return;
  }
  std::string nick = cmd.getParams().front();
  if (_isNickUsed(nick)) {
    client.sendMsg(NumReply::nicknameInUse(cmd));
    return;
  }
  if (client.setNick(nick)) {
    client.sendMsg(NumReply::erroneusNickname(cmd));
    return;
  }

  if (client.getUsername().empty() == false) {
    client.setRegistered();
  }
}

void Server::_handleUSER(const Cmd &cmd) {
  Client &client = cmd.getAuthor();
  if (client.isGoodToRegister() == false) {
    client.sendMsg("ERROR :tried to USER before PASS");
    _disconnectClient(client, "tried to USER before PASS");
    return;
  }
  if (client.isRegistered()) {
    client.sendMsg(NumReply::alreadyRegistered(client));
    return;
  }
  if (cmd.getParams().size() != 4) {
    client.sendMsg(NumReply::needMoreParams(cmd));
    return;
  }
  std::string username = cmd.getParams().front();
  std::string realname = cmd.getParams().back();
  if (realname.size() < 1 || username.size() < 1) {
    client.sendMsg(NumReply::needMoreParams(cmd));
    return;
  }
  client.setUsername(username);
  client.setRealName(realname);
  if (client.getNick() != "*") {
    client.setRegistered();
  }
}

void Server::_handlePING(const Cmd &cmd) {
  Client &client = cmd.getAuthor();
  client.sendMsg("PONG localhost " + cmd.getParams().back());
}

void Server::_handlePRIVMSG(const Cmd &cmd) {
  Client &client = cmd.getAuthor();
  if (client.checkRegistered() == false)
    return;
  if (cmd.getParams().size() != 2) {
    client.sendMsg(NumReply::needMoreParams(cmd));
    return;
  }
  std::string targetNick = cmd.getParams().front();
  std::string msg = cmd.getParams().back();
  const _ClientConstIterator target = _getClient(targetNick);
  if (target == _clients.end()) {
    client.sendMsg(NumReply::noSuchNick(cmd));
    return;
  }
  std::cout << "Target is: " << *target << std::endl;
  std::cout << "It's nick is: " << target->getNick() << std::endl;
  std::cout << "It's Username is: " << target->getUsername() << std::endl;
  std::ostringstream oss;
  oss << ":" << client.getNick() << " ";
  oss << cmd.getName() << " ";
  oss << target->getNick();
  oss << " :" << msg;
  target->sendMsg(oss.str());
}
