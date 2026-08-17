#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/poll.h>

#include "ipc.h"


#define SUNPATH_SIZE 108

/* Standard UDP implementation */
Conn *newConnection(int fd, unsigned int sun_fam, char *sun_path){
    
    Conn *tmp = (Conn *)malloc(sizeof(Conn));
    tmp->fd = fd;
   
    tmp->saddr = (struct sockaddr_un *) malloc(sizeof(struct sockaddr_un));

    memset(tmp->saddr, 0, sizeof(struct sockaddr_un));
    tmp->saddr->sun_family = sun_fam;
    strncpy(tmp->saddr->sun_path, sun_path, SUNPATH_SIZE);

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

Conn* serverOpen(char *sock_file, int sock_type){
    int fd = socket(AF_UNIX, sock_type, 0);
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

/*
 *
 * Expand for TCP
 *
 */

Clients *newTCPClients(Conn *c, int max_clients, int timeout_us) {
    // Allocating in memory
    Clients *cs = (Clients *)malloc(sizeof(Clients));
    cs->fds = (struct pollfd *)calloc(max_clients + 1, sizeof(struct pollfd));

    // Setting main listening socket:
    // This is exactly the same as 
    // cs->fds[0].fd = c->fd;
    cs->fds->fd = c->fd;
    cs->fds->events = POLLIN;

    // Filling in values
    //cs->fds = c->fd;
    cs->nfds = 1;
    cs->max_clients = max_clients;
    cs->timeout = timeout_us;

    // Returning newly built struct
    return cs;
}

int pollRoom(Clients *c) {
    // Poll all fds and check which one is ready
    int ready = poll(c->fds, c->nfds, c->timeout);
    
    // If none are ready then exit
    if(ready == -1) return ready;

    // Assuming all are ready, check for new clients
    if(c->fds[0].revents & POLLIN) {
        int cfd = accept(c->fds->fd, NULL, NULL);
        
        // If we cannot accept the new client then err
        if(cfd == -1)
            return -1;

        // try to add new client
        if(c->nfds < c->max_clients){
            c->nfds += 1;
            c->fds[c->nfds].fd = cfd;
            c->fds[c->nfds].events = POLLIN;
        }

        // Warn about capacity overload
        else {
            perror("Cannot accept new client, max capacity reached !!\n");
            return 1;
        }
    }

    return ready;
}

void broadcast(Clients *c, const char *s) {
    for(int i = 0; i < c->nfds; i++) {
        write(c->fds[i].fd, s, strlen(s));
    }
}

void freeClients(Clients *c){
    free(c->fds);
    free(c);

    c->fds = NULL;
    c = NULL;
}

