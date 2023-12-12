#include "Cmd.hpp"
#include <cstddef>
#include <iomanip>
#include <sstream>
#include <string>
#include "Client.hpp"

Cmd::Cmd(Client &author, std::string content) :_author(author) {
  std::stringstream ss(content);
  ss >> _name;

  size_t pos;
  std::string word;

  while (ss >> word) {
    if (word[0] == ':' && (pos = content.find(" :")) != content.size()) {
      _params.push_back(content.substr(pos + 2));
      break ;
    }
    _params.push_back(word);
  }
}

Cmd::Cmd(const Cmd &copy) : _author(copy._author) { *this = copy; }

Cmd &Cmd::operator=(const Cmd &rhs) {
  _author = rhs._author;
  _name = rhs._name;
  _params = rhs._params;
  return (*this);
}

Cmd::~Cmd(){};

bool Cmd::operator==(const std::string cmdName) const {
  return _name == cmdName;
}
/* ------------------------------------------------------------------------- */

std::string Cmd::getName() const { return (_name); }

std::vector<std::string> Cmd::getParams() const {
  return (_params);
}
Client &Cmd::getAuthor() const {
  return _author;
}

/* ========================================================================= */

typedef std::vector<std::string>::iterator StringIt;

std::ostream &operator<<(std::ostream &stream, const Cmd &msg) {
  stream << msg.getName() << std::endl;
  std::vector<std::string> params = msg.getParams();
  for (StringIt it = params.begin(); it != params.end(); it++) {
    stream << std::setw(4) << "-> ";
    stream << *it << std::endl;
  }
  return (stream);
}
