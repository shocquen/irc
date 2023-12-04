#pragma once

#include "Client.hpp"
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

private:
  Server();
  int _fd;
  std::string _pwd;
  unsigned short _port;
  std::vector<Client> _clients;
  // std::vector<Channel> _channels;
};
