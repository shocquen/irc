#include "Channel.hpp"
#include "Client.hpp"

unsigned long Channel::_idCount = 0;

Channel::Channel(Client &author, const std::string name)
    : _author(author), _name(name) {
  //TODO check if the name is rule compliant
}

Channel::Channel(const Channel &copy) : _author(copy._author) { *this = copy; }

Channel::~Channel(){};

Channel &Channel::operator=(const Channel &rhs) {
  _id = _idCount++;

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
bool Channel::operator==(const std::string name) const {
  return (_name == name);
}

bool Channel::operator==(const Channel &rhs) const {
  return (_id == rhs._id);
}
/* ========================================================================= */

// std::ostream &operator<<(std::ostream &stream, const Channel &c) {
//   stream << "Channel " << c.getName() << std::endl;
//   stream << "members: ";
//   for(Channel::ClientConstIt it = )
  
//   return stream;
// }
