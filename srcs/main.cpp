#include "Client.hpp"
#include "Server.hpp"
#include "irc.hpp"
#include <cstdlib>
#include <iostream>

#define BUFFER_SIZE 2024

int main(void) {
  // Add parsing for port and pwd
  try {
    Server server("LLD", 8080);
    server.run();
    server.stop();
  } catch (Server::ServerException &err) {
    std::cerr << err.what() << std::endl;
    return (EXIT_SUCCESS);
  }

  return (EXIT_SUCCESS);
}
