#include "Client.hpp"
#include "Cmd.hpp"
#include "Server.hpp"
#include "irc.hpp"
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>

#define BUFFER_SIZE 2024

static void argsParser(char **argv, std::string *pwd, unsigned short *port) {
  std::istringstream iss(argv[1]);
  iss >> *port;
  *pwd = argv[2];
}

int main(int argc, char **argv) {
  if (argc != 3)
    return (EXIT_FAILURE);

  unsigned short port;
  std::string pwd;
  argsParser(argv, &pwd, &port);

  try {
    Server server(pwd, port);
    server.run();
    server.stop();
  } catch (Server::ServerException &err) {
    std::cerr << err.what() << std::endl;
    return (EXIT_SUCCESS);
  }

  return (EXIT_SUCCESS);
}
