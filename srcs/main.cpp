#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <string>
#include <strings.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#define PORT 8080
#define TIMEOUTMS 500
#define BUFFER_SIZE 2024
#define MAX_LISTEN 10

int main(void) {
  // struct pollfd pfds[MAX_FD];
  // nfds_t nfds = 1; // The serv has min a socket
  std::vector<struct pollfd> vfds;

/* ------------------------------------------------------------------------- */
/* Bind 1st socket to server                                                 */
/* ------------------------------------------------------------------------- */
  int serverFd = socket(AF_INET, SOCK_STREAM, 0);
  if (serverFd < 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  int opt = 1;
  struct sockaddr_in serverAddress;
  if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  serverAddress.sin_port = htons(PORT);

  if (bind(serverFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
	if (listen(serverFd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

/* ------------------------------------------------------------------------- */

  // Here Paul is at work!
  int ready;
  ssize_t s;
  char buf[BUFFER_SIZE];
  int newSocketFd;
  struct sockaddr_in clientAddress;
  socklen_t addrlen = sizeof(clientAddress);
  while (true) {
    nfds_t nfds = vfds.size() + 1;
    struct pollfd pfds[nfds];

    pfds[0].fd = serverFd;
    pfds[0].events = POLLIN;
    for (nfds_t i = 1; i < nfds; i++) {
      pfds[i] = vfds[i - 1];
    }

    // printf("About to poll()\n");
    ready = poll(pfds, nfds, -1);
    if ( ready == -1) {
      perror("poll");
      exit(EXIT_FAILURE);
    }
    // printf("Ready: %d\n", ready);
    for (nfds_t i = 0; i < nfds; i++) {
      if (!(pfds[i].revents & POLLIN)) continue ;

      if (pfds[i].fd == serverFd) {
      	if ((newSocketFd = accept(serverFd, (struct sockaddr*)&clientAddress, &addrlen)) < 0) {
      		perror("accept");
      		exit(EXIT_FAILURE);
      	} else {
          struct pollfd tmp;
          tmp.fd = newSocketFd;
          tmp.events = POLLIN | POLLOUT;
          vfds.push_back(tmp);
        }
      } else {
        bzero(buf, BUFFER_SIZE);
        printf("About to recv()\n");
        s = recv(pfds[i].fd, buf, BUFFER_SIZE, MSG_DONTWAIT);
        printf("recv() is a succes\n");
        if ( s == -1) {
          perror("read");
          exit(EXIT_FAILURE);
        }
        if (s == 0 ) {
          printf("\tClosing fd %d\n", pfds[i].fd);
          close(pfds[i].fd);
          for (std::vector<struct pollfd>::iterator it = vfds.begin(); it != vfds.end(); it++) {
            if (it->fd == pfds[i].fd) {
              printf("\tEreasing fd %d\n", pfds[i].fd);
              vfds.erase(it);
              break ;
            }
          }
        }
        printf("\tread from %d, %zd bytes: %.*s\n", pfds[i].fd, s, (int)s, buf);
      }
    }
  }


  return (EXIT_SUCCESS);
}
