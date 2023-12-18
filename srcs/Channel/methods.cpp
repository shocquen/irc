#include "Channel.hpp"
#include "Cmd.hpp"
#include "Client.hpp"
#include <algorithm>
#include <sstream>
#include <vector>

void Channel::setName(std::string name) { _name = name; }
void Channel::setTopic(std::string topic) { _name = topic; }
/* ========================================================================= */
void Channel::addMember(Client *m) { _members.push_back(m); }

void Channel::kickMember(const Client &m) {
  std::vector<Client *>::iterator it =
      std::find(_members.begin(), _members.end(), &m);
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
void Channel::toogleOnInvite() { _onInvite = !_onInvite; }

void Channel::toogleTopicRestriction() {
  _topicRestriction = !_topicRestriction;
}

void Channel::toogleOnPwd() { _onPwd = !_onPwd; }
/* ========================================================================= */
Client &Channel::getAuthor() const {
  return _author;
}

std::string Channel::getName() const {
  return _name;
}

std::string Channel::getTopic() const {
  return _topic;
}

std::string Channel::listMembers() const {
  std::ostringstream oss;
  for (_ClientConstIt it = _members.begin(); it != _members.end(); it++) {
    if (hasPerm(*(*it)))
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
  _ClientConstIt target = std::find(_membersBanned.begin(), _membersBanned.end(), &c);
  return (target != _membersBanned.end());
}

bool Channel::isInvitedMember(const Client &c) const {
  _ClientConstIt target = std::find(_membersInveted.begin(), _membersInveted.end(), &c);
  return (target != _membersInveted.end());
}

bool Channel::isOperator(const Client &c) const {
  _ClientConstIt target = std::find(_operators.begin(), _operators.end(), &c);
  return (target != _operators.end());
}

bool Channel::hasPerm(const Client &c) const {
  return (getAuthor() == c || isOperator(c));
}
/* ========================================================================= */
void Channel::sendMsg(std::string msg) const {
  for(_ClientConstIt it = _members.begin(); it != _members.end(); it++) {
    (*it)->sendMsg(msg);
  }
}

void Channel::sendMsg(const Client &author, std::string msg) const {
  for(_ClientConstIt it = _members.begin(); it != _members.end(); it++) {
    if (author == it)
      continue;
    (*it)->sendMsg(msg);
  }
}
