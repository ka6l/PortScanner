#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <common.h>



int createFile(char *filepath){

    int fd;

    if((fd = open(filepath, O_RDONLY)) != -1){
        close(fd);
        return 1; 
    }


    if((fd = open(filepath, O_RDWR | O_CREAT, 0644)) == -1){
            close(fd);
            perror("File Creation");
            return ERROR;
    }


    return 0;
}


int appendFile(const results *result, int entries, const char *filepath) {

    int fd = open(filepath, O_WRONLY | O_APPEND);

    if (fd == -1) {
        perror("open");
        return -1;
    }

    char buf[64];

    for (int i = 0; i < entries; i++) {

        const char *stateStr;

        char state = result[i].state;

        switch(state){
            case OPEN:
                stateStr = "OPEN";
            case CLOSED:
                stateStr = "CLOSED";
            case FILTERED:
                stateStr = "FILTERED";
            default:
                stateStr = "UNKNOWN";
        }

        int len = snprintf(buf, sizeof(buf), "PORT %d %s\n", result[i].port, stateStr);

        if (len < 0 || len >= (int)sizeof(buf)) {
            close(fd);
            return -1;
        }

        ssize_t written = write(fd, buf, len);
        if (written != len) {
            perror("write");
            close(fd);
            return -1;
        }
    }


    close(fd);
    return SUCCESS;
}
