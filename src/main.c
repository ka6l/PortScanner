#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../include/files.h"
#include "../include/common.h"
#include "../include/request.h"

results result[1]; //Declare then resize with realloc

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

    bool set = false;
    bool range = false; 
    bool common = false;
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
                common = true;
                set = true;
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

    
    if(!common && !range){

        int status = sendProbe(target, port, 1);
        checkStatus(status, port, result, 0);

        // printResult(1, result);
        entries++;
    }

    // results result[MAX_WELL_KNOWN_PORTS];
    // memset(&result, 0, sizeof(result));
    
    // while(set){
        
    //     for(int i = 0; i < MAX_WELL_KNOWN_PORTS; i++){
    //         int port = i + 1;
    //         int status = sendProbe(target, port, MAX_WELL_KNOWN_PORTS - 1);
            
    //         if(status == ERROR){
    //             printf("ERROR");
    //         }

    //         if(checkStatus(status, port, result, i) == FILTERED || OPEN){
    //             entries++;
    //         }
    //     }
    //     set = false;
    // }
    
    if(filepath != NULL){

        if(createFile(filepath) == ERROR){
            return ERROR;
        }

        if(appendFile(result, entries, filepath) == -1){
            return ERROR;
        }
    }

    // printResult(entries, result);
    return 0;
    
}