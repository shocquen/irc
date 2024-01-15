#include "Channel.hpp"
#include "Client.hpp"
#include "Cmd.hpp"
#include "Colors.hpp"
#include "NumReply.hpp"
#include "Server.hpp"
#include "Utils.hpp"
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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

  const std::vector<std::string> names = split(cmd.getParams().front(), ",");
  std::vector<std::string> keys;
  if (cmd.getParams().size() > 1) {
    keys = split(cmd.getParams().at(1), ",");
    if (names.size() != keys.size())
      return;
  }

  std::vector<std::string>::const_iterator nameIt;
  size_t index = 0;
  for (nameIt = names.begin();
       nameIt != names.end(); nameIt++, index++) {
    if (nameIt->empty())
      continue;

    _ChannelIt chan = _getChannel(*nameIt);
    const std::string key = index >= keys.size() ? "" : keys[index];
    if (chan != _channels.end()) {
      // The chan already exist
      if (chan->isOnInvite() && chan->isInvitedMember(client) == false) {
        client.sendMsg(NumReply::inviteOnlyChan(client, chan->getName()));
        continue;
      }
      if (chan->getKey() != key) {
        client.sendMsg(NumReply::badChannelKey(chan->getName()));
        continue;
      }
      chan->addMember(&client);
    } else {
      // Create a chan
      _addChannel(client, *nameIt, key);
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
  // Is the target a channel that exist?
  std::string targetName = cmd.getParams().front();
  if (targetName.at(0) != '#')
    return;
  _ChannelIt target = _getChannel(targetName);
  if (target == _channels.end()) {
    client.sendMsg(NumReply::noSushChannel(client, targetName));
    return;
  }

  // If no modestring just reply RPL_CHANNELMODEIS
  if (cmd.getParams().size() == 1) {
    client.sendMsg(NumReply::channelModIs(client, *target));
    return;
  } else if (target->isOperator(client) == false) {
    client.sendMsg(NumReply::chanOPrivsNeeded(client, *target));
    return ;
  }

  const std::string modeStr = cmd.getParams().at(1);
  // Modestring must start with '-' or '+'
  if (modeStr[0] != '+' && modeStr[0] != '-')
    return;
  std::vector<std::string> args;
  if (cmd.getParams().size() == 3)
    args = split(cmd.getParams().at(2), ",");

  const std::string ACCEPTED_MODES = "itkol";
  const std::string PLUSMINUS = "+-";
  const std::string ACCEPTED_CHARS = ACCEPTED_MODES + PLUSMINUS;

  char modeSet = 0;
  size_t index = 0;
  for (std::string::const_iterator it = modeStr.begin(); it != modeStr.end();
       it++) {
    if (ACCEPTED_CHARS.find(*it) == ACCEPTED_CHARS.npos) {
      continue;
    }
    if (PLUSMINUS.find(*it) != PLUSMINUS.npos) {
      modeSet = *it;
    }
    switch (*it) {
    case 'i':
      if ((modeSet == '+' && target->isOnInvite() == false) ||
          (modeSet == '-' && target->isOnInvite() == true)) {
        target->toggleOnInvite();
      }
      break;
    case 't':
      if ((modeSet == '+' && target->isTopicProtected() == false) ||
          (modeSet == '-' && target->isTopicProtected() == true)) {
        target->toggleTopicProtection();
      }
      break;
    case 'k':
      if (modeSet == '-') {
        target->setKey("");
      } else if (args.size() >= index) {
        target->setKey(args[index]);
      }
      break;
    case 'l':
      if (modeSet == '+' && args.size() >= index) {
        // target->setMemberLimit(std::stoul(args[index]));
        target->setMemberLimit(std::strtoul(args[index].c_str(), NULL, 10));
      } else if (modeSet == '-') {
        target->setMemberLimit(0);
      }
      break;
    }
    if (*it == 'o' && args.size() >= index) {
      _ClientIt it = _getClient(args[index]);
      if (modeSet == '-') {
        target->rmOperator(*it);
      } else {
        target->addOperator(&(*it));
      }
    }
    if (ACCEPTED_MODES.find(*it) != ACCEPTED_MODES.npos) {
      index++;
    }
  }
  std::ostringstream ctx;
  std::ostringstream ctxArgs;
  if (target->isOnInvite())
    ctx << "i";
  if (target->isTopicProtected())
    ctx << "t";
  if (target->getKey().empty() == false)
    ctx << "k";
  if (target->getMemberLimit() != 0) {
    ctx << "l ";
    ctxArgs << target->getMemberLimit();
  }
  target->sendMsg(":localhost MODE " + target->getName() + " " + ctx.str() + " " + ctxArgs.str());
}
