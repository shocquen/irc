#include "Colors.hpp"
#include "irc.hpp"

#include <arpa/inet.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
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

void sendMsg(int fd, std::string msg) {
    std::cout << FGRN("-> ") << msg << std::endl;
    msg += "\r\n";
    send(fd, msg.c_str(), msg.size(), MSG_DONTWAIT);
}

int main(int argc, char **argv) {
    if (argc != 4) {
        std::cerr << "Usage: ./bot <host> <port> <psw>" << std::endl;
        return EXIT_FAILURE;
    }
    const Args args(argv);

    int                status, valread, serverFd;
    struct sockaddr_in serv_addr;

    char buffer[1024] = {0};
    if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(args.port);

    // Convert IPv4 and IPv6 addresses from text to binary
    if (inet_pton(AF_INET, args.host.c_str(), &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if ((status = connect(serverFd, (struct sockaddr *)&serv_addr,
                          sizeof(serv_addr))) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    sendMsg(serverFd, "Hello");
    printf("Hello message sent\n");
    valread = read(serverFd, buffer,
                   1024 - 1); // subtract 1 for the null
                              // terminator at the end
    printf("%s\n", buffer);
    (void)valread;

    // closing the connected socket
    close(serverFd);
    return EXIT_SUCCESS;
}
