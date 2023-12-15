#pragma once

#include <ostream>
#include <string>
#include <vector>

class Client;

class Cmd {
public:
  Cmd(Client &author, std::string content);
  Cmd(const Cmd &copy);
  ~Cmd();
  Cmd &operator=(const Cmd &rhs);

  bool operator==(const std::string cmdName) const;

  std::string getName() const;
  std::vector<std::string> getParams() const;
  Client &getAuthor() const;

private:
  Cmd();
  Client &_author;
  std::string _name;
  std::vector<std::string> _params;
  // std::map<std::string, > map of cmd and ptr on func
};

std::ostream &operator<<(std::ostream &stream, const Cmd &msg);
