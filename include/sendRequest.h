#ifndef SEND_REQUEST
#define SEND_REQUEST

#include <poll.h>
#include "common.h"

int handlePolling(int FD, struct pollfd *FDS);
int sendProbe(char *target, int port, int numOfPorts);

#endif