#pragma once

#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <string>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <sys/poll.h>

typedef struct pollfd pollfd_t;
extern int status_g;
