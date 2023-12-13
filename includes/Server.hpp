#pragma once

#include "Client.hpp"
#include "Cmd.hpp"
#include <string>
#include <vector>

class Server {
public:
  Server(std::string pwd, unsigned short port);
  Server(const Server &copy);
  ~Server();
  Server &operator=(const Server &rhs);

  void run();
  void stop();

  class ServerException : public std::exception {
  private:
    std::string _msg;

  public:
    virtual ~ServerException() throw();
    ServerException(std::string const msg);
    ServerException(std::string const msg, int errnoValue);
    virtual const char *what() const throw();
  };
  typedef void (Server::*_cmdFuncPtr)(const Cmd &);

private:
  Server();
  int _fd;
  std::string _pwd;
  unsigned short _port;
  std::vector<Client> _clients;
  // std::vector<Channel> _channels;
/* ------------------------------------------------------------------------- */
  typedef std::vector<Client>::iterator _ClientIterator;
/* ------------------------------------------------------------------------- */
  void _acceptNewClient();
  void _disconnectClient(Client &client, std::string ctx);
  // Return 1 if there is one or more complete msgs to treat.
  // Else return 0.
  int _readFromClient(const _ClientIterator &client);
/* ------------------------------------------------------------------------- */
  void _handlePASS(const Cmd &cmd);
  void _handleNICK(const Cmd &cmd);
  void _handleUSER(const Cmd &cmd);
  // void _handleCAP(const Cmd &cmd);

  static std::map<std::string, Server::_cmdFuncPtr> initCmdHandlers();
  const static std::map<std::string, _cmdFuncPtr> _cmdHandlers;
};
