#include "Server.hpp"
#include "NumReply.hpp"

std::map<std::string, Server::_cmdFuncPtr> Server::initCmdHandlers() {
  std::map<std::string, Server::_cmdFuncPtr> m;
  m["PASS"] = &Server::_handlePASS;
  return m;
}
const std::map<std::string, Server::_cmdFuncPtr> Server::_cmdHandlers =
    Server::initCmdHandlers();

/* ========================================================================= */

void Server::_handlePASS(const Cmd &cmd) {
  Client client = cmd.getAuthor();
  if (cmd.getParams().empty()) {
    client.sendMsg(NumReply::needMoreParams(cmd));
    return;
  }
  if (client.isAuth()) {
    client.sendMsg(NumReply::alreadyRegistered(cmd));
    return;
  }
  if (cmd.getParams().front() != _pwd) {
    client.sendMsg(NumReply::passwdMismatch(cmd));
    return;
  }
  client.validatePwd();
  std::cout << "client[" << client.getId() << "] validate PASS" << std::endl;
}
// void Server::_handleNICK(const Cmd &cmd) {
//   Client client = cmd.getAuthor();
// }
// void Server::_handleUSER(const Cmd &cmd) {}
