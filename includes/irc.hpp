#pragma once

#include <algorithm>
#include <cerrno>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <stdio.h>
#include <string>
#include <strings.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

typedef struct pollfd pollfd_t;
extern int            status_g;
