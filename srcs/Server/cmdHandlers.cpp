#include "Cmd.hpp"
#include "NumReply.hpp"
#include "Server.hpp"
#include <iostream>
#include <sstream>

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
  Client &client = cmd.getAuthor();
  if (cmd.getParams().empty()) {
    client.sendMsg(NumReply::needMoreParams(cmd));
    return;
  }
  if (client.isAuth()) {
    client.sendMsg(NumReply::alreadyRegistered());
    return;
  }
  if (cmd.getParams().front() != _pwd) {
    client.sendMsg(NumReply::passwdMismatch());
    return;
  }
  client.validatePwd();
  std::cout << "client[" << client.getId() << "] validate PASS" << std::endl;
}

void Server::_handleNICK(const Cmd &cmd) {
  Client &client = cmd.getAuthor();
  if (client.isGoodToAuth() == false) {
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
  std::ostringstream cl;
  cl << client;
  if (client.setNick(nick)) {
    client.sendMsg(NumReply::erroneusNickname(cmd));
    return;
  }
  std::cout << cl.str() << " changed his nickname to " << nick << std::endl;

  if (client.getUsername().empty() == false) {
    
  }
}

void Server::_handleUSER(const Cmd &cmd) {
  Client &client = cmd.getAuthor();
  if (client.isGoodToAuth() == false) {
    _disconnectClient(client, "tried to USER before PASS");
    return ;
  }
}
