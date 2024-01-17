#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
void check_host_name(
    int hostname) { // This function returns host name for local computer
    if (hostname == -1) {
        perror("gethostname");
        exit(1);
    }
}
void check_host_entry(
    struct hostent *hostentry) { // find host info from host name
    if (hostentry == NULL) {
        perror("gethostbyname");
        exit(1);
    }
}
void IP_formatter(char *IPbuffer) { // convert IP string to dotted decimal
                                    // format
    if (NULL == IPbuffer) {
        perror("inet_ntoa");
        exit(1);
    }
}
