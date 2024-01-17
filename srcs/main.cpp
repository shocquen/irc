#include "Channel.hpp"
#include "Cmd.hpp"
#include "Server.hpp"
#include "irc.hpp"

#include <csignal>
#include <sstream>

#define BUFFER_SIZE 2024
int status_g = 0;

static void argsParser(char **argv, std::string *pwd, unsigned short *port) {
    std::istringstream iss(argv[1]);
    // TODO check if all argv[1] is digit and if it superior at 0;
    // askljdksja dasdj  jaskljd klasjdkl jasldjlasjd jajklas jklasjd kl
    // jkldjasl jlkasdjlkasjd ljasdljj
    iss >> *port;
    *pwd = argv[2];
}

static void sigintHandler(int signum) {
    status_g = signum;
}

int main(int argc, char **argv) {
    if (argc != 3)
        return (EXIT_FAILURE);

    signal(SIGINT, sigintHandler);
    unsigned short port;
    std::string    pwd;
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
