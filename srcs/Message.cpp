#include "Message.hpp"
#include <sstream>
#include <string>

Message::Message(Client &author, std::string content) : _author(author) {
  // TODO Parsing content into _cmd and _params

  (void)content;
  // std::stringstream ss(content);

  // ss >> _cmd;

  // std::string word;
  // while (ss >> word && ss.peek() != ':') {
  // }
}

Message::Message(const Message &copy) : _author(copy._author) { *this = copy; }

Message &Message::operator=(const Message &rhs) {
  _author = rhs._author;
  _cmd = rhs._cmd;
  _params = rhs._params;
  return (*this);
}

Message::~Message(){};

/* ------------------------------------------------------------------------- */

std::string Message::getCmd() const { return (_cmd); }
