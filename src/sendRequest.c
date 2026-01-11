#include <poll.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../include/common.h"

/*
    Loop through common most ports
    Store Results

*/


int sendProbe(char *target, int port, int numOfPorts){

    int MAX = 5;
    int optVal = 1;

    struct sockaddr_in info;
    struct pollfd FDS[numOfPorts]; // Calculate the range to get the actually number going to be used 

    
    //Clear structure
    memset(&info, 0, sizeof(info));

    info.sin_port = htons(port);
    info.sin_family = AF_INET;
    info.sin_addr.s_addr = inet_addr(target);

    int FD = socket(PF_INET, SOCK_STREAM, 0);

    if(FD == -1){
        perror("Socket");
        return -1;
    }

    //Sets to non blocking
    int flags = fcntl(FD, F_GETFL, 0);
    fcntl(FD, F_SETFL, flags | O_NONBLOCK);


    memset(FDS, 0, sizeof(FDS));
    FDS[0].fd = FD;
    FDS[0].events = POLLOUT;


    

    int connectFd = connect(FD, (struct sockaddr_in*)&info, sizeof(info));

    if(connectFd == 0){
        // printf("First connect... Port open\n");
        close(FD);
        return OPEN;
    }

    else if(connectFd == -1 && errno == EINPROGRESS){

        int NFDS = poll(FDS, 1, 1000);
        if(NFDS == 0){
            // printf("POLL FILTERED");
            close(FD);
            return FILTERED;
        }

 
        else if(NFDS > 0 && FDS[0].revents & (POLLOUT | POLLERR | POLLHUP)){
            socklen_t optLen = sizeof(optVal);


            if(getsockopt(FD, SOL_SOCKET, SO_ERROR, &optVal, &optLen) < 0){
                perror("GETSOCKOPT");
                return -1;
            }

            if(optVal == 0){
                // printf("Second check... port open");
                close(FD);
                return OPEN;
            }
            else if(optVal == ECONNREFUSED){
                // printf("Second check... port closed");
                close(FD);
                return CLOSED;
                
            }
            else{
                // printf("Filtered");
                close(FD);
                return FILTERED;
            }

            

        printf("FDS");
            return -1;
        }
    
    }

  return -1;

}