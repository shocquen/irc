#include "Channel.hpp"
#include "Client.hpp"
#include "Cmd.hpp"
#include "Colors.hpp"
#include "NumReply.hpp"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <vector>

void Channel::setName(std::string name) {
    _name = name;
}

void Channel::setKey(std::string key) {
    _key = key;
}

// setTopic and send it to members
void Channel::setTopic(std::string topic) {
    _topic = topic;
    for (_ClientConstIt it = _members.begin(); it != _members.end(); it++) {
        (*it)->sendMsg(NumReply::topic(*(*it), *this));
    }
}

void Channel::setMemberLimit(unsigned long limit) {
    _memberLimit = limit;
}
/* ========================================================================= */
void Channel::addMember(Client *m) {
    _members.push_back(m);
}

void Channel::rmMember(const Client &client) {
    if (_members.empty())
        return;
    _ClientIt it = std::find(_members.begin(), _members.end(), &client);
    if (it != _members.end())
        _members.erase(it);
}

void Channel::kickMember(const Client &m, const Client &op, std::string ctx) {
    std::vector<Client *>::iterator it =
        std::find(_members.begin(), _members.end(), &m);
    if (it == _members.end())
        return;
    if (ctx.empty())
        ctx = "is kicked";

    sendMsg(":" + op.getNick() + "!" + op.getUsername() + "@localhost" +
            " KICK " + getName() + " " + m.getNick() + " :" + ctx);
    _members.erase(it);
}

void Channel::kickAll() {
    while (_members.size()) {
        if (_members.front()->getId() != _author.getId())
            kickMember(*_members.front(), _author, "Author left chan");
    }
}

void Channel::inviteMember(Client *m) {
    _membersInvited.push_back(m);
}

void Channel::unInviteMember(const Client &m) {
    _ClientIt it =
        std::find(_membersInvited.begin(), _membersInvited.end(), &m);
    if (it != _membersInvited.end())
        _membersInvited.erase(it);
}

void Channel::banMember(Client *m) {
    _membersBanned.push_back(m);
}

void Channel::unBanMember(const Client &m) {
    std::vector<Client *>::iterator it =
        std::find(_membersBanned.begin(), _membersBanned.end(), &m);
    _membersBanned.erase(it);
}
/* ------------------------------------------------------------------------- */
void Channel::addOperator(Client *m) {
    std::cout << KGRN << _name << ": new modo: " << m->getNick() << RST
              << std::endl;
    _operators.push_back(m);
}

void Channel::rmOperator(const Client &client) {
    if (_operators.empty())
        return;
    _ClientIt it = std::find(_operators.begin(), _operators.end(), &client);
    if (it != _operators.end()) {
        std::cout << KRED << _name << ": rm modo: " << client.getNick() << RST
                  << std::endl;
        _operators.erase(it);
    }
}
/* ========================================================================= */
void Channel::toggleOnInvite() {
    _onInvite = !_onInvite;
}

void Channel::toggleTopicProtection() {
    _topicProtection = !_topicProtection;
}

/* ========================================================================= */
unsigned long Channel::getId() const {
    return _id;
}

Client &Channel::getAuthor() const {
    return _author;
}

std::string Channel::getName() const {
    return _name;
}

std::string Channel::getKey() const {
    return _key;
}

std::string Channel::getTopic() const {
    return _topic;
}

std::string Channel::listMembers() const {
    std::ostringstream oss;
    for (_ClientConstIt it = _members.begin(); it != _members.end(); it++) {
        if (ClientHasPriv(*(*it)))
            oss << "@";
        oss << (*it)->getNick() << " ";
    }
    return oss.str();
}

unsigned long Channel::getMemberLimit() const {
    return _memberLimit;
}
size_t Channel::getMemberCount() const {
    return _members.size();
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
        std::find(_membersInvited.begin(), _membersInvited.end(), &c);
    return (target != _membersInvited.end());
}

bool Channel::isOperator(const Client &c) const {
    _ClientConstIt target = std::find(_operators.begin(), _operators.end(), &c);
    return (target != _operators.end());
}

bool Channel::ClientHasPriv(const Client &c) const {
    return (getAuthor() == c || isOperator(c));
}

bool Channel::isTopicProtected() const {
    return _topicProtection;
}
bool Channel::isOnInvite() const {
    return _onInvite;
}
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
