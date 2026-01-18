#ifndef COMMON_H
#define COMMON_H

#define ERROR -1
#define SUCCESS 1
#define MAX_PORT 65535
#define MAX_WELL_KNOWN_PORTS 1023

typedef enum{
    OPEN,
    CLOSED,
    FILTERED    
} portStates;

typedef struct {
    int port;
    portStates state;
} results;




#endif