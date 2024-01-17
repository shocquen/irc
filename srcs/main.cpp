#include "Channel.hpp"
#include "Cmd.hpp"
#include "Server.hpp"
#include "irc.hpp"

#include <csignal>
#include <sstream>
#include <string>

#define BUFFER_SIZE 2024
int status_g = 0;

typedef struct Args {
    Args(char **data) {
        std::istringstream iss(data[1]);
        iss >> port;
        pwd = data[2];
    }
    unsigned short port;
    std::string    pwd;
} Args;

static void sigintHandler(int signum) {
    status_g = signum;
}

int main(int argc, char **argv) {
    if (argc != 3)
        return (EXIT_FAILURE);
    signal(SIGINT, sigintHandler);
    Args args(argv);
    try {
        Server server(args.pwd, args.port);
        server.run();
        server.stop();
    } catch (Server::ServerException &err) {
        std::cerr << err.what() << std::endl;
        return (EXIT_SUCCESS);
    }

    return (EXIT_SUCCESS);
}
