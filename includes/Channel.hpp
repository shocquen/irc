#pragma once

#include "Cmd.hpp"
#include <cstddef>
#include <ostream>
#include <string>
class Channel {
public:
  Channel(Client &author, const std::string name, const std::string key);
  Channel(const Channel &copy);
  ~Channel();
  Channel &operator=(const Channel &rhs);

  bool operator==(const std::string name) const;
  bool operator==(unsigned long id) const;
  bool operator==(const Channel &rhs) const;
/* ========================================================================= */
  void addMember(Client *);
  void rmMember(const Client &client);
  void kickMember(const Client &member, const Client &op, std::string);
  void kickAll();
  void banMember(Client *);
  void unBanMember(const Client &);
  void inviteMember(Client *);
  void unInviteMember(const Client &);
/* ------------------------------------------------------------------------- */
  void addOperator(Client *);
  void rmOperator(const Client &);
/* ------------------------------------------------------------------------- */
  void toggleOnInvite();
  void toggleTopicProtection();
/* ------------------------------------------------------------------------- */
  void setName(std::string);
  void setKey(std::string);
  void setTopic(std::string topic);
  void setMemberLimit(unsigned long memberLimit);
/* ========================================================================= */
  unsigned long getId() const;
  Client &getAuthor() const;
  std::string getName() const;
  std::string getKey() const;
  std::string getTopic() const;
  std::string listMembers() const;
  unsigned long getMemberLimit() const;
  size_t getMemberCount() const;
/* ------------------------------------------------------------------------- */
  bool isMember(const Client &) const;
  bool isBannedMember(const Client &) const;
  bool isInvitedMember(const Client &) const;
  bool isOperator(const Client &)const;
  bool ClientHasPriv(const Client &) const;
  bool isTopicProtected() const;
  bool isOnInvite() const;
/* ========================================================================= */
  void sendMsg(std::string msg) const;
  void sendMsg(const Client &author, std::string msg) const;
/* ========================================================================= */
private:
  Channel();
  static unsigned long _idCount;

  Client              &_author;
  std::string          _name;
  std::string          _key;
  std::string          _topic;
  std::vector<Client*> _members;
  std::vector<Client*> _membersBanned;
  std::vector<Client*> _membersInvited;
  std::vector<Client*> _operators;
  unsigned long        _id;
  unsigned long        _memberLimit;
  bool                 _onInvite;
  bool                 _topicProtection;
/* ========================================================================= */
  typedef std::vector<Client*>::const_iterator _ClientConstIt;
  typedef std::vector<Client*>::iterator _ClientIt;
};

// std::ostream &operator<<(std::ostream stream, const Channel &c); 

