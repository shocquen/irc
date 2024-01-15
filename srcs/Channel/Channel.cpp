#include "Channel.hpp"
#include "Client.hpp"

unsigned long Channel::_idCount = 0;

Channel::Channel(Client &author, const std::string name, const std::string key)
    : _author(author), _name(name), _key(key) {
  _id = _idCount++;

  _memberLimit = 0;
  _onInvite = false;
  _topicProtection = true;
}

Channel::Channel(const Channel &copy) : _author(copy._author) { *this = copy; }

Channel::~Channel(){};

Channel &Channel::operator=(const Channel &rhs) {
  _id = _idCount++;
  // _id = rhs._id;

  _author = rhs._author;
  _name = rhs._name;
  _key = rhs._key;
  _members = rhs._members;
  _membersInveted = rhs._membersInveted;
  _operators = rhs._operators;
  _memberLimit = rhs._memberLimit;
  _onInvite = rhs._onInvite;
  _topicProtection = rhs._topicProtection;
  return *this;
}

/* ========================================================================= */
bool Channel::operator==(const std::string name) const {
  return (_name == name);
}

bool Channel::operator==(const Channel &rhs) const {
  return (_id == rhs._id);
}
/* ========================================================================= */

// std::ostream &operator<<(std::ostream &stream, const Channel &c) {
//   stream << "Channel " << c.getName() << std::endl;
//   stream << "key: " << c.getKey() << std::endl;
//   return stream;
// }

