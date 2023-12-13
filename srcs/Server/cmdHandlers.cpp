#include "Server.hpp"
#include "NumReply.hpp"
#include <iostream>

std::map<std::string, Server::_cmdFuncPtr> Server::initCmdHandlers() {
  std::map<std::string, Server::_cmdFuncPtr> m;
  m["PASS"] = &Server::_handlePASS;
  m["NICK"] = &Server::_handleNICK;
  return m;
}

const std::map<std::string, Server::_cmdFuncPtr> Server::_cmdHandlers =
    Server::initCmdHandlers();

/* ========================================================================= */

void Server::_handlePASS(const Cmd &cmd) {
  Client client = cmd.getAuthor();
  if (cmd.getParams().empty()) {
    client.sendMsg(NumReply::needMoreParams(cmd));
    return;
  }
  if (client.isAuth()) {
    client.sendMsg(NumReply::alreadyRegistered(cmd));
    return;
  }
  if (cmd.getParams().front() != _pwd) {
    client.sendMsg(NumReply::passwdMismatch(cmd));
    return;
  }
  client.validatePwd();
  std::cout << "client[" << client.getId() << "] validate PASS" << std::endl;
}

void Server::_handleNICK(const Cmd &cmd) {
  Client client = cmd.getAuthor();
  if (client.isGoodToAuth() == false) {
    _disconnectClient(client, "tried to NICK before PASS");
    return ;
  }
  std::cout << client.isGoodToAuth() << std::endl;
  client.sendMsg(NumReply::nicknameInUse(cmd));
}
// void Server::_handleUSER(const Cmd &cmd) {}
