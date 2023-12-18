#include "Channel.hpp"
#include "Client.hpp"
#include "Cmd.hpp"
#include "NumReply.hpp"
#include "Server.hpp"
#include <cstddef>
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
  m["TOPIC"] = (Server::CmdMiddleWare){true, &Server::_handleTOPIC};
  m["KICK"] = (Server::CmdMiddleWare){true, &Server::_handleKICK};
  m["NAMES"] = (Server::CmdMiddleWare){true, &Server::_handleNAMES};
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
/* ------------------------------------------------------------------------- */
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
  oss << ":" << client.getNick() << "!" << client.getUsername() << "@localhost ";
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
/* ------------------------------------------------------------------------- */
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
       ":" + client.getNick() + "!" + client.getUsername() + "@localhost "
       + "JOIN " + chan->getName()
  );
  if (chan->getTopic().empty() == false)
    client.sendMsg(NumReply::topic(client, *chan));
  client.sendMsg(NumReply::namReply(client, *chan));
  client.sendMsg(NumReply::endOfNames(client, *chan));
}

void Server::_handleTOPIC(const Cmd &cmd) {
  Client &client = cmd.getAuthor();
  if (cmd.getParams().empty()) {
    client.sendMsg(NumReply::needMoreParams(cmd));
    return;
  }
  std::string chanName = cmd.getParams().front();
  _ChannelIt chan = _getChannel(chanName);
  if (chan == _channels.end()) {
    client.sendMsg(NumReply::noSushChannel(client, chanName));
  }
  if (chan->isMember(client) == false) {
    client.sendMsg(NumReply::notOnChannel(client, *chan));
    return;
  }

  if (cmd.getParams().size() == 1) { // client wanna get the topic
    if (chan->getTopic().empty()) {
      client.sendMsg(NumReply::noTopic(client, *chan));
      return ;
    }
    client.sendMsg(NumReply::topic(client, *chan));
  } else if (cmd.getParams().size() == 2) { // client wanna change the topic
    if (chan->isTopicProtected() && chan->ClientHasPriv(client) == false) {
      client.sendMsg(NumReply::chanOPrivsNeeded(client, *chan));
      return;
    }
    std::string newTopic = cmd.getParams().back();
    chan->setTopic(newTopic);
  }
}

void Server::_handleKICK(const Cmd &cmd) {
  Client &client = cmd.getAuthor();
  if (cmd.getParams().size() < 2) {
    client.sendMsg(NumReply::needMoreParams(cmd));
    return;
  }
  std::string chanName = cmd.getParams().front();
  _ChannelIt chan = _getChannel(chanName);
  if (chan == _channels.end()) {
    client.sendMsg(NumReply::noSushChannel(client, chanName));
    return;
  }

  if (chan->ClientHasPriv(client) == false) {
    client.sendMsg(NumReply::chanOPrivsNeeded(client, *chan));
    return;
  }

  std::string targetParam = cmd.getParams().at(1);
  std::string ctx = cmd.getParams().size() > 2 ? cmd.getParams().back() : "";

  size_t end = targetParam.find(",");
  while (end != targetParam.npos) {
    std::string targetNick = targetParam.substr(0, end);
    targetParam.erase(0, end + 1);
    _ClientConstIt target = _getConstClient(targetNick);
    if (target == _clients.end())
      continue;
    if (chan->isMember(*target) == false) {
      client.sendMsg(NumReply::userNotInChannel(client, target->getNick(), *chan));
      continue;
    }
    if (chan->ClientHasPriv(*target))
      continue;
    chan->kickMember(*target, client, ctx);
  }

  _ClientConstIt target = _getConstClient(targetParam);
  if (target == _clients.end())
    return;
  if (chan->isMember(*target) == false) {
    client.sendMsg(NumReply::userNotInChannel(client, target->getNick(), *chan));
    return;
  }
  if (chan->ClientHasPriv(*target)) // Dont kick an Op
    return;
  chan->kickMember(*target, client, ctx);
}

void Server::_handleNAMES(const Cmd &cmd) {
  Client &client = cmd.getAuthor();
  if (cmd.getParams().empty()) {
    client.sendMsg(NumReply::needMoreParams(cmd));
    return;
  }
  std::string name = cmd.getParams().front();
  _ChannelIt chan = _getChannel(name);
  if (chan == _channels.end()) {
    client.sendMsg(NumReply::noSushChannel(client, name));
    return;
  }

  client.sendMsg(NumReply::namReply(client, *chan));
}
