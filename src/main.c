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
        default: return "UNKNOWN";
    }
}

void printResult(int MAX, results *result){

    for(int i = 0; i < MAX; i++){
        int index = i + 1;
        printf("PORT %d: %s\n", result[i].port, StateToString(result[i].state));
    }
}

int checkStatus(int status, int port, results result[], int pos){

        switch(status){
            
            case -1:
                printf("ERROR\n");
                return ERROR;

            case OPEN:
                result[pos].port = port;
                result[pos].state = OPEN;

                printf("Port %d: OPEN\n", port);
                break;
                
            case FILTERED:
                result[pos].port = port;
                result[pos].state = FILTERED;

                printf("Port %d: FILTERED\n", port);
                break;
            
        }

    return 0;
}


int main(int argc, char *argv[]){

    bool range = false; 
    bool common = false;
    bool set = true;
    int ch;
    int port = 0;
    int portCount = 0;
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
        results result[1];

        int status = sendProbe(target, port, 1);
        checkStatus(status, port, result, 0);

        printResult(1, result);


        return 0;
    }

    results result[MAX_WELL_KNOWN_PORTS];
    memset(&result, 0, sizeof(result));
    
    while(set){
        
        for(int i = 0; i < MAX_WELL_KNOWN_PORTS; i++){
            int port = i + 1;
            int status = sendProbe(target, port, MAX_WELL_KNOWN_PORTS - 1);
            
            if(status == ERROR){
                printf("ERROR");
            }

            int check = checkStatus(status, port, result, i);

            if(check == FILTERED || OPEN){
                portCount++;
            }
        }
        set = false;
    }
    
    printResult(portCount, result);
    return 0;
    
}