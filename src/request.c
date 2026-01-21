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


int checkStatus(int status, int port, results result[], int pos){

        switch(status){
            
            case -1:
                printf("ERROR\n");
                return ERROR;

            case OPEN:
                result[pos].port = port;
                result[pos].state = OPEN;

                return 1;  

            case FILTERED:
                result[pos].port = port;
                result[pos].state = FILTERED;
                
                return 1;

            case CLOSED:
                break;
            
        }

    return ERROR;
}


int sendProbe(char *target, int port, int numOfPorts){

    int optVal = 1;

    struct sockaddr_in info;
    struct pollfd FDS; // Calculate the range to get the actually number going to be used 

    
    //Clear structure
    memset(&info, 0, sizeof(info));

    info.sin_port = htons(port);
    info.sin_family = AF_INET;
    info.sin_addr.s_addr = inet_addr(target);

    int FD = socket(PF_INET, SOCK_STREAM, 0);

    if(FD == -1){
        perror("Socket");
        return ERROR;
    }

    //Sets to non blocking
    int flags = fcntl(FD, F_GETFL, 0);
    fcntl(FD, F_SETFL, flags | O_NONBLOCK);


    memset(&FDS, 0, sizeof(FDS));
    FDS.fd = FD;
    FDS.events = POLLOUT;


    

    int connectFd = connect(FD, (struct sockaddr_in*)&info, sizeof(info));

    if(connectFd == 0){
        // printf("First connect... Port open\n");
        close(FD);
        return OPEN;
    }

    else if(connectFd == -1 && errno == EINPROGRESS){

        int NFDS = poll(&FDS, 1, 1000);
        if(NFDS == 0){
            // printf("POLL FILTERED");
            close(FD);
            return FILTERED;
        }

 
        else if(NFDS > 0 && FDS.revents & (POLLOUT | POLLERR | POLLHUP)){
            socklen_t optLen = sizeof(optVal);


            if(getsockopt(FD, SOL_SOCKET, SO_ERROR, &optVal, &optLen) < 0){
                perror("GETSOCKOPT");
                close(FD);
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