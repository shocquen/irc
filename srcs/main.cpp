#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 4242
#define TIMEOUTMS 500
#define MAX_FD 2
#define BUFFER_SIZE 2024
#define MAX_LISTEN 10

int main(void) {
  struct pollfd pfds[MAX_FD];
  nfds_t nfds = 1; // The serv has min a socket

/* ------------------------------------------------------------------------- */
/* Bind 1st socket to server                                                 */
/* ------------------------------------------------------------------------- */
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  int opt = 1;
  struct sockaddr_in address;
  socklen_t addrlen = sizeof(address);
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
/* ------------------------------------------------------------------------- */

	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

  // Now we need PAUL! 
  int ready;
  ssize_t s;
  char buf[10];
  while (true) {
    printf("About to poll()\n");
    ready = poll(pfds, nfds, -1);
    if ( ready == -1) {
      perror("poll");
      exit(EXIT_FAILURE);
    }
    printf("Ready: %d\n", ready);

    for (nfds_t i = 0; i < nfds; i++) {
      if (pfds[i].events & POLLIN) {
        s = read(pfds[i].fd, buf, sizeof(buf));
        if ( s == -1) {
          perror("read");
          exit(EXIT_FAILURE);
        }
        printf("\tread %zd bytes: %.*s\n", s, (int)s, buf);
      } else { // POLLERR | POLLHUP
        printf("\tclosing fd %d\n", pfds[i].fd);
        if (close(pfds[i].fd) == -1) {
          perror("close");
          exit(EXIT_FAILURE);
        }
      }
    }
  }


  return (EXIT_SUCCESS);
}
