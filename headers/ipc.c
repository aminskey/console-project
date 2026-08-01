#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

#include "ipc.h"

#define SUNPATH_SIZE 128



Conn *newConnection(int fd, unsigned int sun_fam, char *sun_path){
    
    Conn *tmp = (Conn *)malloc(sizeof(Conn));
    tmp->fd = fd;
   
    tmp->saddr = (struct sockaddr_un *) malloc(sizeof(struct sockaddr_un));

    memset(tmp->saddr, 0, sizeof(struct sockaddr_un));
    tmp->saddr->sun_family = sun_fam;
    strcpy(tmp->saddr->sun_path, sun_path);

    return tmp;
}

void freeConnection(Conn *c){
    free(c->saddr);
    free(c);

    c = NULL;
}

int serverClose(Conn *c){
    if(close(c->fd) == -1){
        perror("Cannot close server :(");
        return -1;
    }

    if(unlink(c->saddr->sun_path) == -1){
        perror("Cannot unlink path");
        return -1;
    }

    freeConnection(c);
    return 0;
}

Conn* serverOpen(char *sock_file){
    int fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if(fd == -1) { 
        perror("Cannot create socket :(");
        return NULL; 
    }

    Conn* connection = newConnection(fd, AF_UNIX, sock_file);
    if(!connection) {
        perror("Cannot create connection object");
        return NULL;
    }

    if(bind(fd, 
           (const struct sockaddr*) connection->saddr, 
           sizeof(struct sockaddr_un)) < 0){
        perror("Cannot bind address to socket");
        return NULL;
    }

    return connection;
}
