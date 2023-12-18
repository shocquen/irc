#pragma once

#include "Cmd.hpp"
#include <ostream>
#include <string>
class Channel {
public:
  Channel(Client &author, const std::string name);
  Channel(const Channel &copy);
  ~Channel();
  Channel &operator=(const Channel &rhs);

  bool operator==(const std::string name) const;
  bool operator==(const Channel &rhs) const;
/* ========================================================================= */
  void addMember(Client *);
  void kickMember(const Client &);
  void banMember(Client *);
  void unBanMember(const Client &);
  void inviteMember(Client *);
  void unInviteMember(const Client &);
/* ------------------------------------------------------------------------- */
  void addOperator(Client *);
  void rmOperator(const Client &);
/* ------------------------------------------------------------------------- */
  void toogleOnInvite();
  void toogleTopicRestriction();
  void toogleOnPwd();
/* ------------------------------------------------------------------------- */
  void setName(std::string);
  void setTopic(std::string topic);
/* ========================================================================= */
  Client &getAuthor() const;
  std::string getName() const;
  std::string getTopic() const;
  std::string listMembers() const;
/* ------------------------------------------------------------------------- */
  bool isMember(const Client &) const;
  bool isBannedMember(const Client &) const;
  bool isInvitedMember(const Client &) const;
  bool isOperator(const Client &)const;
  bool hasPerm(const Client &) const;
/* ========================================================================= */
  void sendMsg(std::string msg) const;
  void sendMsg(const Client &author, std::string msg) const;
/* ========================================================================= */
private:
  Channel();
  static unsigned long _idCount;

  Client              &_author;
  std::string          _name;
  std::string          _pwd;
  std::string          _topic;
  std::vector<Client*> _members;
  std::vector<Client*> _membersBanned;
  std::vector<Client*> _membersInveted;
  std::vector<Client*> _operators;
  unsigned long        _id;
  unsigned long        _memberLimit;
  bool                 _onInvite;
  bool                 _topicRestriction;
  bool                 _onPwd;
/* ========================================================================= */
  typedef std::vector<Client*>::const_iterator _ClientConstIt;
  typedef std::vector<Client*>::iterator _ClientIt;
};

// std::ostream &operator<<(std::ostream stream, const Channel &c); 

