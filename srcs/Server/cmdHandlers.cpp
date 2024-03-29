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
    ret["PASS"]    = (Server::CmdMiddleWare){.mustRegistered = false,
                                             .func = &Server::_handlePASS};
    ret["NICK"]    = (Server::CmdMiddleWare){false, &Server::_handleNICK};
    ret["USER"]    = (Server::CmdMiddleWare){false, &Server::_handleUSER};
    ret["PING"]    = (Server::CmdMiddleWare){true, &Server::_handlePING};
    ret["PRIVMSG"] = (Server::CmdMiddleWare){true, &Server::_handlePRIVMSG};
    ret["JOIN"]    = (Server::CmdMiddleWare){true, &Server::_handleJOIN};
    ret["TOPIC"]   = (Server::CmdMiddleWare){true, &Server::_handleTOPIC};
    ret["KICK"]    = (Server::CmdMiddleWare){true, &Server::_handleKICK};
    ret["NAMES"]   = (Server::CmdMiddleWare){true, &Server::_handleNAMES};
    ret["MODE"]    = (Server::CmdMiddleWare){true, &Server::_handleMODE};
    ret["INVITE"]  = (Server::CmdMiddleWare){true, &Server::_handleINVITE};
    ret["QUIT"]    = (Server::CmdMiddleWare){true, &Server::_handleQUIT};
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
    std::string oldNick = client.getNick();
    if (client.setNick(nick)) {
        client.sendMsg(NumReply::erroneusNickname(cmd));
        return;
    } else if (client.isRegistered()) {
        for (_ClientConstIt it = _clients.begin(); it != _clients.end(); it++) {
            if (*it == client)
                continue;
            it->sendMsg(":" + oldNick + "!" + client.getUsername() +
                        "@localhost NICK " + client.getNick());
        }
        client.sendMsg(":" + oldNick + " NICK " + client.getNick());
    }

    if (client.getUsername().empty() == false &&
        client.isRegistered() == false) {
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
    if (client.getNick() != "*" && client.isRegistered() == false) {
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
    std::string        targetNick = cmd.getParams().front();
    std::string        msg        = cmd.getParams().back();
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
    std::vector<std::string>       keys;
    if (cmd.getParams().size() > 1) {
        keys = split(cmd.getParams().at(1), ",");
        if (names.size() != keys.size())
            return;
    }

    std::vector<std::string>::const_iterator nameIt;
    size_t                                   index = 0;
    for (nameIt = names.begin(); nameIt != names.end(); nameIt++, index++) {
        if (nameIt->empty())
            continue;

        _ChannelIt        chan = _getChannel(*nameIt);
        const std::string key  = index >= keys.size() ? "" : keys[index];
        if (chan != _channels.end()) {
            // The chan already exist
            if (chan->getMemberLimit() &&
                chan->getMemberCount() >= chan->getMemberLimit()) {
                client.sendMsg(NumReply::channelIsFull(client, *chan));
                return;
            }
            if (chan->isOnInvite() && chan->isInvitedMember(client) == false) {
                client.sendMsg(
                    NumReply::inviteOnlyChan(client, chan->getName()));
                continue;
            }
            if (chan->getKey() != key) {
                client.sendMsg(
                    NumReply::badChannelKey(client, chan->getName()));
                continue;
            }
            chan->addMember(&client);
            if (chan->isInvitedMember(client)) {
                chan->unInviteMember(client);
            }
        } else {
            // Create a chan
            _addChannel(client, *nameIt, key);
            chan = _getChannel(*nameIt);
        }

        chan->sendMsg(":" + client.getNick() + "!" + client.getUsername() +
                      "@localhost " + "JOIN " + chan->getName());
        if (chan->getTopic().empty() == false)
            client.sendMsg(NumReply::topic(client, *chan));
        client.sendMsg(NumReply::nameReply(client, *chan));
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
    _ChannelIt  chan     = _getChannel(chanName);
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
    _ChannelIt  chan     = _getChannel(chanName);
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
    _ChannelIt  chan = _getChannel(name);
    if (chan == _channels.end()) {
        client.sendMsg(NumReply::noSushChannel(client, name));
        return;
    }

    client.sendMsg(NumReply::nameReply(client, *chan));
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
    _ChannelIt chan = _getChannel(targetName);
    if (chan == _channels.end()) {
        client.sendMsg(NumReply::noSushChannel(client, targetName));
        return;
    }

    // If no modestring just reply RPL_CHANNELMODEIS
    if (cmd.getParams().size() == 1) {
        client.sendMsg(NumReply::channelModIs(client, *chan));
        return;
    } else if (chan->ClientHasPriv(client) == false) {
        client.sendMsg(NumReply::chanOPrivsNeeded(client, *chan));
        return;
    }

    const std::string modeStr = cmd.getParams().at(1);
    // Modestring must start with '-' or '+'
    if (modeStr[0] != '+' && modeStr[0] != '-')
        return;
    std::vector<std::string> args;
    for (size_t i = 2; i < cmd.getParams().size(); i++) {
        args.push_back(cmd.getParams()[i]);
    }

    const std::string ACCEPTED_MODES = "itkol";
    const std::string PLUSMINUS      = "+-";
    const std::string ACCEPTED_CHARS = ACCEPTED_MODES + PLUSMINUS;

    char               modeCtx  = 0;
    size_t             argIndex = 0;
    std::ostringstream ctx;
    std::ostringstream ctxArgs;
    for (std::string::const_iterator modeChar = modeStr.begin();
         modeChar != modeStr.end(); modeChar++) {
        if (ACCEPTED_CHARS.find(*modeChar) == ACCEPTED_CHARS.npos) {
            continue;
        }
        if (PLUSMINUS.find(*modeChar) != PLUSMINUS.npos) {
            modeCtx = *modeChar;
        }

        switch (*modeChar) {
        case 'i':
            if ((modeCtx == '+' && chan->isOnInvite() == false) ||
                (modeCtx == '-' && chan->isOnInvite() == true)) {
                chan->toggleOnInvite();
                ctx << modeCtx << "i";
            }
            break;
        case 't':
            if ((modeCtx == '+' && chan->isTopicProtected() == false) ||
                (modeCtx == '-' && chan->isTopicProtected() == true)) {
                chan->toggleTopicProtection();
                ctx << modeCtx << "t";
            }
            break;
        case 'k':
            if (modeCtx == '-') {
                chan->setKey("");
                ctx << "-k";
            } else if (args.size() > argIndex) {
                chan->setKey(args[argIndex]);
                argIndex++;
                ctx << "+k";
            }
            break;
        case 'l':
            if (modeCtx == '+' && args.size() > argIndex) {
                // target->setMemberLimit(std::stoul(args[index]));
                chan->setMemberLimit(
                    std::strtoul(args[argIndex].c_str(), NULL, 10));
                argIndex++;
                ctx << "+l";
                if (ctxArgs.tellp())
                    ctxArgs << " ";
                ctxArgs << chan->getMemberLimit();
            } else if (modeCtx == '-') {
                ctx << "-l";
                chan->setMemberLimit(0);
            }
            break;
        }
        if (*modeChar == 'o' && args.size() > argIndex) {
            std::string tmpClientName = args[argIndex];
            argIndex++;
            _ClientIt target = _getClient(tmpClientName);
            if (target == _clients.end()) {
                client.sendMsg(
                    NumReply::userNotInChannel(client, tmpClientName, *chan));
            } else {
                if (modeCtx == '-') {
                    chan->rmOperator(*target);
                    ctx << "-o";
                    if (ctxArgs.tellp())
                        ctxArgs << " ";
                    ctxArgs << target->getNick();
                } else {
                    chan->addOperator(&(*target));
                    ctx << "+o";
                    if (ctxArgs.tellp())
                        ctxArgs << " ";
                    ctxArgs << target->getNick();
                }
            }
        }
    }
    chan->sendMsg(":" + client.getNick() + " MODE " + chan->getName() + " " +
                  ctx.str() + " " + ctxArgs.str());
}

void Server::_handleINVITE(const Cmd &cmd) {
    Client &client = cmd.getAuthor();

    if (cmd.getParams().size() != 2) {
        client.sendMsg(NumReply::needMoreParams(cmd));
        return;
    }

    _ChannelIt chan = _getChannel(cmd.getParams().at(1));
    if (chan == _channels.end()) {
        client.sendMsg(NumReply::noSushChannel(client, cmd.getParams().at(1)));
        return;
    }
    if (chan->isMember(client) == false) {
        client.sendMsg(NumReply::notOnChannel(client, *chan));
        return;
    }
    if (chan->isOnInvite() && chan->ClientHasPriv(client) == false) {
        client.sendMsg(NumReply::chanOPrivsNeeded(client, *chan));
        return;
    }

    _ClientIt invitedClient = _getClient(cmd.getParams().front());
    if (invitedClient == _clients.end()) {
        return;
    }
    if (chan->isMember(*invitedClient)) {
        client.sendMsg(
            NumReply::userOnChannel(client, invitedClient->getNick(), *chan));
        return;
    }

    chan->inviteMember(&(*invitedClient));
    client.sendMsg(NumReply::inviting(client, invitedClient->getNick(), *chan));
    invitedClient->sendMsg(":" + client.getNick() + "@localhost INVITE " +
                           invitedClient->getNick() + " " + chan->getName());
}

void Server::_handleQUIT(const Cmd &cmd) {
    Client &client = cmd.getAuthor();

    _disconnectClient(client,
                      cmd.getParams().empty() ? "" : cmd.getParams().front());
}
