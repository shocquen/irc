#include "Channel.hpp"
#include "Cmd.hpp"
#include "Colors.hpp"
#include "Server.hpp"
#include "irc.hpp"

#include <csignal>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

#define BUFFER_SIZE 2024
int status_g = 0;

typedef struct Args {
    Args(char **data) {
        std::istringstream iss(data[1]);
        if (*data[1]) {
            iss >> port;
        } else {
            port = 0;
        }
        if (iss.peek() != EOF)
            port = 0;
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
    if (args.port == 0) {
        std::cerr << "ircserv: port number invalid: " << Color::red(argv[1])
                  << std::endl;
        return EXIT_FAILURE;
    }
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
