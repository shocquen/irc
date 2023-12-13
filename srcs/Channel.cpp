#include "Channel.hpp"
#include "Client.hpp"

Channel::Channel(Client &author, const std::string name)
    : _author(author), _name(name) {
  //TODO check if the name is rule compliant
}

Channel::Channel(const Channel &copy) : _author(copy._author) { *this = copy; }

Channel::~Channel(){};

Channel &Channel::operator=(const Channel &rhs) {
  _author = rhs._author;
  _name = rhs._name;
  _pwd = rhs._pwd;
  _members = rhs._members;
  _membersInveted = rhs._membersInveted;
  _operators = rhs._operators;
  _id = rhs._id;
  _memberLimit = rhs._memberLimit;
  _onInvite = rhs._onInvite;
  _topicRestriction = rhs._topicRestriction;
  _onPwd = rhs._onPwd;
  return *this;
}

/* ========================================================================= */


