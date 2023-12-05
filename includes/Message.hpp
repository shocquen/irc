#pragma once

#include "Client.hpp"
#include <string>
#include <vector>
class Message {
public:
  Message(Client &author, std::string content);
  Message(const Message &copy);
  ~Message();
  Message &operator=(const Message &rhs);

  std::string getCmd() const;

private:
  Message();
  Client &_author;
  std::string _cmd;
  std::vector<std::string> _params;
};
