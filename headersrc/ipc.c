#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/poll.h>

#include "../headers/ipc.h"


#define SUNPATH_SIZE 108

// This is the main socket for connections
int ms_index = 0;


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

// TODO: REWRITE
int accept_new_clients(clients_t *t) {
    // Poll all fds and check which one is ready
    int ready = poll(t->fds, t->nfds, t->timeout_us);

    // If none are ready then exit
    if(ready == -1 || ms_index == -1) 
        return -1;

    // Assuming all are ready, check for new clients
    if(t->fds[0].revents & POLLIN) {
        int cfd = accept(t->fds[0].fd, NULL, NULL);

        // If we cannot accept the new client then err
        if(cfd == -1){
            printf("Cannot accept new client\n");
            return -1;
        }

        struct pollfd tmp = {
            .fd = cfd,
            .events = POLLIN | POLLHUP,
            .revents = 0
        };

        // Warn about capacity overload
        if(push(t, tmp) < 0) {
            perror("Cannot accept new client, max capacity reached !!\n");
            return 1;
        }

        printf("> Accepted new client\n");
        printf("   fd-number: %d\n", cfd);
        printf("   Current number of clients: %lu\n\n", t->nfds);
    }


    return ready;
}
//
// void broadcast(Clients *c, const char *s) {
//     for(int i = 0; i < c->nfds; i++) {
//         write(c->fds[i].fd, s, strlen(s));
//     }
// }
//
// void freeClients(Clients *c){
//     free(c->fds);
//     free(c);
//
//     c->fds = NULL;
//     c = NULL;
// }

