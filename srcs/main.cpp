#include "Client.hpp"
#include "Server.hpp"
#include "irc.hpp"
#include <cstdlib>
#include <iostream>

#define BUFFER_SIZE 2024

int main(void) {
  try {
    Server server("LLD", 4242);
    server.run();
    server.stop();
  } catch (Server::ServerException err) {
    std::cerr << err.what() << std::endl;
    return (EXIT_SUCCESS);
  }

  return (EXIT_SUCCESS);
}
