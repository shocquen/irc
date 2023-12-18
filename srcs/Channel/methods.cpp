#include "Channel.hpp"
#include "Client.hpp"
#include "Cmd.hpp"
#include "NumReply.hpp"
#include <algorithm>
#include <sstream>
#include <vector>

void Channel::setName(std::string name) { _name = name; }

// setTopic and send it to members
void Channel::setTopic(std::string topic) {
  _topic = topic;
  for (_ClientConstIt it = _members.begin(); it != _members.end(); it++) {
    (*it)->sendMsg(NumReply::topic(*(*it), *this));
  }
}
/* ========================================================================= */
void Channel::addMember(Client *m) { _members.push_back(m); }

void Channel::kickMember(const Client &m, const Client &op, std::string ctx) {
  std::vector<Client *>::iterator it =
      std::find(_members.begin(), _members.end(), &m);
  if (ctx.empty())
    ctx = "is kicked";

  sendMsg(":" + op.getNick() + "!" + op.getUsername() + "@localhost" + " KICK " + getName() + " " + m.getNick() + " :" +
          ctx);
  _members.erase(it);
}

void Channel::inviteMember(Client *m) { _membersInveted.push_back(m); }

void Channel::unInviteMember(const Client &m) {
  std::vector<Client *>::iterator it =
      std::find(_membersInveted.begin(), _membersInveted.end(), &m);
  _membersInveted.erase(it);
}

void Channel::banMember(Client *m) { _membersBanned.push_back(m); }

void Channel::unBanMember(const Client &m) {
  std::vector<Client *>::iterator it =
      std::find(_membersBanned.begin(), _membersBanned.end(), &m);
  _membersBanned.erase(it);
}
/* ------------------------------------------------------------------------- */
void Channel::addOperator(Client *m) { _operators.push_back(m); }

void Channel::rmOperator(const Client &m) {
  std::vector<Client *>::iterator it =
      std::find(_operators.begin(), _operators.end(), &m);
  _operators.erase(it);
}
/* ========================================================================= */
void Channel::toggleOnInvite() { _onInvite = !_onInvite; }

void Channel::toggleTopicProtection() { _topicProtection = !_topicProtection; }

void Channel::toggleOnPwd() { _onPwd = !_onPwd; }
/* ========================================================================= */
Client &Channel::getAuthor() const { return _author; }

std::string Channel::getName() const { return _name; }

std::string Channel::getTopic() const { return _topic; }

std::string Channel::listMembers() const {
  std::ostringstream oss;
  for (_ClientConstIt it = _members.begin(); it != _members.end(); it++) {
    if (ClientHasPriv(*(*it)))
      oss << "@";
    oss << (*it)->getNick() << " ";
  }
  return oss.str();
}
/* ------------------------------------------------------------------------- */
bool Channel::isMember(const Client &c) const {
  _ClientConstIt target = std::find(_members.begin(), _members.end(), &c);
  return (target != _members.end());
}

bool Channel::isBannedMember(const Client &c) const {
  _ClientConstIt target =
      std::find(_membersBanned.begin(), _membersBanned.end(), &c);
  return (target != _membersBanned.end());
}

bool Channel::isInvitedMember(const Client &c) const {
  _ClientConstIt target =
      std::find(_membersInveted.begin(), _membersInveted.end(), &c);
  return (target != _membersInveted.end());
}

bool Channel::isOperator(const Client &c) const {
  _ClientConstIt target = std::find(_operators.begin(), _operators.end(), &c);
  return (target != _operators.end());
}

bool Channel::ClientHasPriv(const Client &c) const {
  return (getAuthor() == c || isOperator(c));
}

bool Channel::isTopicProtected() const { return _topicProtection; }
/* ========================================================================= */
void Channel::sendMsg(std::string msg) const {
  for (_ClientConstIt it = _members.begin(); it != _members.end(); it++) {
    (*it)->sendMsg(msg);
  }
}

void Channel::sendMsg(const Client &author, std::string msg) const {
  if (isMember(author) == false) {
    author.sendMsg(NumReply::notOnChannel(author, *this));
    return;
  }
  for (_ClientConstIt it = _members.begin(); it != _members.end(); it++) {
    if (author == it)
      continue;
    (*it)->sendMsg(msg);
  }
}
