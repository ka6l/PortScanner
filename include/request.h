#ifndef REQUEST
#define REQUEST

#include <poll.h>
#include "common.h"

int handlePolling(int FD, struct pollfd *FDS);
int sendProbe(char *target, int port, int numOfPorts);

#endif