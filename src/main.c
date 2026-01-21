#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../include/files.h"
#include "../include/common.h"
#include "../include/request.h"

results *result = NULL;

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




int main(int argc, char *argv[]){

    bool range = false; 
    bool commonPorts = false;
    int ch;
    int start;
    int end;
    int port = 0;
    int entries = 0;
    char *target = NULL;
    char *filepath = NULL;



    if(argc < 2){
        printf("Missing args");
        return -1;
    }

    while((ch = getopt(argc, argv, "p:ct:s:") )!= -1){
        
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
                commonPorts = true;
                break;
            
            case 's':
                filepath = optarg;
                break;
            
            case 'r':
                range = true;
                break;
                // Figure out how to find range

            case '?':
                usage();
                break;

            default:
                usage();
                return -1;
        }
    }

    if(port != 0 && commonPorts){
        printf("Can't have common ports and single port flagged");
        return ERROR;
    }
    
    result = malloc(sizeof *result * MAX_WELL_KNOWN_PORTS);
    if (!result) {
        perror("malloc");
        return ERROR;
    }

    memset(result, 0, sizeof *result * MAX_WELL_KNOWN_PORTS);


    if(port > 0){

        int request = sendProbe(target, port, 1);
        if(request == ERROR){
            return ERROR;
        }

        result[0].port = port;
        result[0].state = request;
        entries++;
    }

    
    if(commonPorts){
        entries = 0;

        
        for(int i = 0; i < MAX_WELL_KNOWN_PORTS; i++){
            int port = i + 1;
            int request = sendProbe(target, port, MAX_WELL_KNOWN_PORTS - 1);
            
            if(request == ERROR){
                printf("ERROR");
                return ERROR;
            }

            if(request == FILTERED || request == OPEN){

                result[entries].port = port;
                result[entries].state = request;
                
                entries++;
            }
        }
    }

    
    if(filepath != NULL){

        if(createFile(filepath) == ERROR){
            return ERROR;
        }

        if(appendFile(result, entries, filepath) == -1){
            return ERROR;
        }
    }

    free(result);
    return 0;
    
}