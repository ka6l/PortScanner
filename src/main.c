#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../include/common.h"
#include "../include/sendRequest.h"


void usage(){
    printf("Usage:\n");
    printf("-p (port) | -t (target)| -c (common ports) | -s (save output)\n");
}

const char *StateToString(portStates s){
    switch(s){
        case OPEN: return "OPEN";
        case CLOSED: return "CLOSED";
        case FILTERED: return "FILTERED";
    }
}

void printResult(int MAX, results *result){

    for(int i = 1; i < MAX; i++){
        printf("PORT %d: %s\n", result->port, StateToString(result->state));
    }
}

int checkStatus(int status, int port, results result[]){

        switch(status){
            
            case -1:
                printf("ERROR\n");
                return ERROR;

            case OPEN:
                result->port = port;
                result->state = OPEN;

                printf("Port %d: OPEN\n", port);
                return 0;
                
            case FILTERED:
                result->port = port;
                result->state = FILTERED;

                printf("Port %d: FILTERED\n", port);
                return 0;   
            
        }

    return -1;
}


int main(int argc, char *argv[]){

    bool range = false; 
    bool common = false;
    bool set = true;
    int ch;
    int port = 0;
    char *target = NULL;
    char *filepath = NULL;


    if(argc < 2){
        printf("Missing args");
        return -1;
    }

    while((ch = getopt(argc, argv, "p:ct:") )!= -1){
        
        switch(ch){

            case 't':
                target = optarg;
                break;

            case 'p':
                port = atoi(optarg);

                if(port <= 0 || port > MAX_PORT){
                    printf("Invalid Port\n");
                    return -1;
                }

                break;

            case 'c':
                common = true;
                break;

            case '?':
                usage();
                break;

            default:
                usage();
                return -1;
        }
    }





    if(!common && !range){
        results *result[1];

        int status = sendProbe(&target, port, 1);
        checkStatus(status, port, &result);

        printResult(1, &result);


        return 0;
    }

    
    while(set){
        results *result[MAX_WELL_KNOWN_PORTS - 1];
        
        for(int i = 1; i < MAX_WELL_KNOWN_PORTS; i++){
            int status = sendProbe(&target, i, MAX_WELL_KNOWN_PORTS - 1);
            
            if(status == ERROR){
                printf("ERROR");
            }

            checkStatus(status, i, &result);
        }
        printResult(MAX_WELL_KNOWN_PORTS, &result);

    }

    return 0;
    
}