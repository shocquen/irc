#include "Channel.hpp"
#include "Cmd.hpp"
#include "NumReply.hpp"
#include "Server.hpp"
#include <iostream>
#include <sstream>

std::map<std::string, Server::CmdMiddleWare> Server::initCmdHandlers() {
  std::map<std::string, Server::CmdMiddleWare> m;
  m["PASS"] = (Server::CmdMiddleWare){.mustRegistered = false,
                                      .func = &Server::_handlePASS};
  m["NICK"] = (Server::CmdMiddleWare){false, &Server::_handleNICK};
  m["USER"] = (Server::CmdMiddleWare){false, &Server::_handleUSER};
  m["PING"] = (Server::CmdMiddleWare){true, &Server::_handlePING};
  m["PRIVMSG"] = (Server::CmdMiddleWare){true, &Server::_handlePRIVMSG};
  m["JOIN"] = (Server::CmdMiddleWare){true, &Server::_handleJOIN};
  return m;
}

const std::map<std::string, Server::CmdMiddleWare> Server::_cmdHandlers =
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
  if (cmd.getParams().size() != 2) {
    client.sendMsg(NumReply::needMoreParams(cmd));
    return;
  }
  std::string targetNick = cmd.getParams().front();
  std::string msg = cmd.getParams().back();
  std::ostringstream oss;
  oss << ":" << client.getNick() << " ";
  oss << cmd.getName() << " ";
  if (targetNick[0] == '#') {
    const _ChannelConstIt target = _getConstChannel(targetNick);
    if (target == _channels.end()) {
      client.sendMsg(NumReply::noSuchNick(cmd));
      return;
    }
    oss << target->getName();
    oss << " :" << msg;
    target->sendMsg(client, oss.str());
  } else {
    const _ClientConstIt target = _getConstClient(targetNick);
    if (target == _clients.end()) {
      client.sendMsg(NumReply::noSuchNick(cmd));
      return;
    }
    oss << target->getNick();
    oss << " :" << msg;
    target->sendMsg(oss.str());
  }
}

void Server::_handleJOIN(const Cmd &cmd) {
  Client &client = cmd.getAuthor();
  if (cmd.getParams().empty()) {
    client.sendMsg(NumReply::needMoreParams(cmd));
    return;
  }
  
  std::string name = cmd.getParams().front();
  _ChannelIt chan = _getChannel(name);
  if (chan != _channels.end()) { // There is already a Chan
    chan->addMember(&client);
  } else {
    Channel newChan(client, name);
    newChan.addMember(&client);
    _channels.push_back(newChan);
    chan = _getChannel(newChan);
  }
  chan->sendMsg(
       ":" + client.getNick() + " "
       + "JOIN " + chan->getName()
  );
  if (chan->getTopic().empty() == false)
    client.sendMsg(NumReply::topic(client, *chan));
  client.sendMsg(NumReply::namReply(client, *chan));
  client.sendMsg(NumReply::endOfNames(client, *chan));
}
