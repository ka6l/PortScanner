#ifndef REQUEST
#define REQUEST

#include <poll.h>
#include "common.h"

int sendProbe(char *target, int port, int numOfPorts);
int checkStatus(int status, int port, results result[], int pos);

#endif