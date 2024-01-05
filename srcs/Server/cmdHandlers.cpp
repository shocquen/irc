#include "Channel.hpp"
#include "Client.hpp"
#include "Cmd.hpp"
#include "NumReply.hpp"
#include "Server.hpp"
#include "Utils.hpp"
#include <cstddef>
#include <iostream>
#include <sstream>
#include <vector>
#define ACCEPTED_MODES "itkol"

std::map<std::string, Server::CmdMiddleWare> Server::initCmdHandlers() {
  std::map<std::string, Server::CmdMiddleWare> ret;
  ret["PASS"] = (Server::CmdMiddleWare){.mustRegistered = false,
                                      .func = &Server::_handlePASS};
  ret["NICK"] = (Server::CmdMiddleWare){false, &Server::_handleNICK};
  ret["USER"] = (Server::CmdMiddleWare){false, &Server::_handleUSER};
  ret["PING"] = (Server::CmdMiddleWare){true, &Server::_handlePING};
  ret["PRIVMSG"] = (Server::CmdMiddleWare){true, &Server::_handlePRIVMSG};
  ret["JOIN"] = (Server::CmdMiddleWare){true, &Server::_handleJOIN};
  ret["TOPIC"] = (Server::CmdMiddleWare){true, &Server::_handleTOPIC};
  ret["KICK"] = (Server::CmdMiddleWare){true, &Server::_handleKICK};
  ret["NAMES"] = (Server::CmdMiddleWare){true, &Server::_handleNAMES};
  ret["MODE"] = (Server::CmdMiddleWare){true, &Server::_handleMODE};
  return ret;
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
  oss << ":" << client.getNick() << "!" << client.getUsername()
      << "@localhost ";
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

  std::vector<std::string> names = split(cmd.getParams().front(), ",");
  std::vector<std::string> keys;
  if (cmd.getParams().size() > 1) {
    keys = split(cmd.getParams().at(1), ",");
    if (names.size() != keys.size())
      return;
  }
  std::vector<std::string>::const_iterator nameIt, keyIt;
  for (nameIt = names.begin(), keyIt = keys.begin();
       nameIt != names.end() && keyIt != keys.end(); nameIt++, keyIt++) {
    if (nameIt->empty())
      continue;

    _ChannelIt chan = _getChannel(*nameIt);
    if (chan != _channels.end()) { 
      // The chan already exist
      // TODO check onIvite chan
      if (chan->getKey() == *keyIt)
        chan->addMember(&client);
      else {
        client.sendMsg(NumReply::badChannelKey(chan->getName()));
        continue;
      }
    } else {
      // Create a chan
      _addNewChannel(client, *nameIt, *keyIt);
      chan = _getChannel(*nameIt);
    }

    chan->sendMsg(":" + client.getNick() + "!" + client.getUsername() +
                  "@localhost " + "JOIN " + chan->getName());
    if (chan->getTopic().empty() == false)
      client.sendMsg(NumReply::topic(client, *chan));
    client.sendMsg(NumReply::namReply(client, *chan));
    client.sendMsg(NumReply::endOfNames(client, *chan));
  }
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
      return;
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

  std::string ctx = cmd.getParams().size() > 2 ? cmd.getParams().back() : "";
  std::vector<std::string> targetsNick = split(cmd.getParams().at(1), ",");
  std::vector<std::string>::const_iterator it;
  for (it = targetsNick.begin(); it != targetsNick.end(); it++) {
    _ClientConstIt target = _getConstClient(*it);
    if (target == _clients.end())
      continue;
    if (chan->isMember(*target) == false) {
      client.sendMsg(
          NumReply::userNotInChannel(client, target->getNick(), *chan));
      continue;
    }
    if (chan->ClientHasPriv(*target))
      continue;
    chan->kickMember(*target, client, ctx);
  }
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

void Server::_handleMODE(const Cmd &cmd) {
  Client &client = cmd.getAuthor();
  if (cmd.getParams().empty()) {
    client.sendMsg(NumReply::needMoreParams(cmd));
    return;
  }
  std::string targetName = cmd.getParams().front();
  if (targetName.at(0) != '#')
    return;
  _ChannelIt target = _getChannel(targetName);
  if (target == _channels.end()) {
    client.sendMsg(NumReply::noSushChannel(client, targetName));
    return;
  }
  
  if (cmd.getParams().size() == 1) { // GET target's mode 
    client.sendMsg(NumReply::channelModIs(client, *target));
  } else { // SET target's mode
    std::string modeStr = cmd.getParams().at(1);
    std::string modeArgs = cmd.getParams().size() > 1 ? cmd.getParams().at(2) : "";
    char mode;
    for (int i = 0; i < modeStr.size(); i++) {
      
    }
  }
}
