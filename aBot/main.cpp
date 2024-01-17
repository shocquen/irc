#include "Colors.hpp"
#include "irc.hpp"

#include <algorithm>
#include <arpa/inet.h>
#include <cctype>
#include <csignal>
#include <cstdlib>
#include <ctype.h>
#include <iostream>
#include <list>
#include <ostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

typedef std::list<std::string>::iterator ListStrIt;

int         SERVER_FD = -1;
static void sigintHandler(int signum) {
    (void)signum;
    if (SERVER_FD) {
        close(SERVER_FD);
        SERVER_FD = -1;
    }
}
typedef struct Args {
    std::string    host;
    std::string    pwd;
    unsigned short port;
    Args(char **data) {
        host = data[1];
        std::istringstream iss(data[2]);
        iss >> port;
        pwd = data[3];
    }
} Args;

void sendMsg(std::string msg) {
    std::cout << FGRN("-> ") << msg << std::endl;
    msg += "\r\n";
    send(SERVER_FD, msg.c_str(), msg.size(), MSG_DONTWAIT);
}

std::string BUFFER;
int         readMsgs(int fd, std::list<std::string> &msgs) {
    ssize_t s;
    char    buf[BUFFER_SIZE];

    msgs.clear();
    bzero(buf, BUFFER_SIZE);
    s = recv(fd, buf, BUFFER_SIZE - 1, 0);
    if (s == -1) {
        perror("recv");
        return -1;
    }

    if (s) {
        BUFFER += buf;
        std::string::size_type pos;
        while ((pos = BUFFER.find("\r\n")) != BUFFER.npos) {
            msgs.push_back(BUFFER.substr(0, pos));
            BUFFER = BUFFER.substr(pos + 2, BUFFER.size());
        }
        ListStrIt it;
        for (it = msgs.begin(); it != msgs.end(); it++) {
            std::cout << FCYN("<- ") << *it << std::endl;
        }
    }
    return msgs.size();
}

int main(int argc, char **argv) {
    if (argc != 4) {
        std::cerr << "Usage: ./bot <host> <port> <psw>" << std::endl;
        return EXIT_FAILURE;
    }
    const Args args(argv);

    int                status;
    struct sockaddr_in serv_addr;

    if ((SERVER_FD = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return EXIT_FAILURE;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(args.port);

    // Convert IPv4 and IPv6 addresses from text to binary
    if (inet_pton(AF_INET, args.host.c_str(), &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return EXIT_FAILURE;
    }

    if ((status = connect(SERVER_FD, (struct sockaddr *)&serv_addr,
                          sizeof(serv_addr))) < 0) {
        printf("\nConnection Failed \n");
        return EXIT_FAILURE;
    }

    sendMsg("PASS " + args.pwd);
    sendMsg("NICK Astarion");
    sendMsg("USER Astarion Astarion 127.0.0.1 :Astarion Ancunín");
    std::list<std::string> msgs;
    if ((status = readMsgs(SERVER_FD, msgs)) == -1)
        return EXIT_FAILURE;
    else if (status) {
        if (msgs.front().find(":Welcome to ft_irc") == std::string::npos)
            return EXIT_FAILURE;
        else
            std::cout << FGRN("Registered!") << std::endl;
    }
    signal(SIGINT, sigintHandler);
    while (true) {
        if ((status = readMsgs(SERVER_FD, msgs)) == -1)
            return EXIT_FAILURE;
        else if (status == 0)
            continue;
        if (msgs.front() == "ERROR :server closing")
            break;
        for (ListStrIt it = msgs.begin(); it != msgs.end(); it++) {
            std::string::size_type pos;
            if ((pos = it->find("PRIVMSG Astarion :")) == it->npos)
                continue;
            std::string content = it->substr(pos + 18);
            pos                 = it->find("!");
            std::string author  = it->substr(1, pos - 1);
            for (std::string::iterator charIt = content.begin();
                 charIt != content.end(); charIt++) {
                *charIt = tolower(*charIt);
            }
            if (content.find("hello") != content.npos) {
                sendMsg("PRIVMSG " + author +
                        " :Hello you, what an exquisite scent!");
                sendMsg("PRIVMSG " + author +
                        " :I cannot promise to not byte you... Well if you "
                        "concent of course eheh...\n(yes/no)?");

            } else if (content.find("yes") != content.npos) {
                sendMsg("PRIVMSG " + author + " :Then I'll see you at night");
            } else if (content.find("no") != content.npos) {
                sendMsg("PRIVMSG " + author +
                        " :I dont think I have any reasons to stay. Bye!");
                close(SERVER_FD);
                return 2;
            }
        }
    }

    // closing the connected socket
    if (SERVER_FD)
        close(SERVER_FD);
    return EXIT_SUCCESS;
}
