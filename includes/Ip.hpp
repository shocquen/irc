#pragma once

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void check_host_name(int hostname);
void check_host_entry(struct hostent *hostentry);
void IP_formatter(char *IPbuffer);

typedef struct Host {
    char           *IP;
    char            name[256];
    struct hostent *host_entry;
    int             hostname;
    Host() {
        hostname = gethostname(name, sizeof(name)); // find the host name
        check_host_name(hostname);
        host_entry = gethostbyname(name);           // find host information
        check_host_entry(host_entry);

        IP = inet_ntoa(*((struct in_addr *)host_entry
                             ->h_addr_list[0])); // Convert into IP string
    }
} Host;
