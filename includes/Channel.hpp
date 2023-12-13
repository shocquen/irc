#pragma once

#include "Cmd.hpp"
#include <string>
class Channel {
public:
  Channel(Client &author, const std::string name);
  Channel(const Channel &copy);
  ~Channel();
  Channel &operator=(const Channel &rhs);

  void setAuthor(std::string);
  void setName(std::string);
  void addMember(const Client &);
  void kickMember(const Client &);
  void addOperator(const Client &);
  void rmOperator(const Client &);
  void toogleOnInvite();
  void toogleTopicRestriction();
  void toogleOnPwd();
/* ------------------------------------------------------------------------- */
  Client &getAuthor() const;
  Client &getMember(const std::string nick) const;
  bool    isMember(const Client &) const;
/* ------------------------------------------------------------------------- */
  void sendMsg(std::string msg) const;

private:
  Channel();
  static unsigned long _idCount;

  Client              &_author;
  std::string          _name;
  std::string          _pwd;
  std::vector<Client*> _members;
  std::vector<Client*> _membersInveted;
  std::vector<Client*> _operators;
  unsigned long        _id;
  unsigned long        _memberLimit;
  bool                 _onInvite;
  bool                 _topicRestriction;
  bool                 _onPwd;
};
