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

extern struct pollfd *table_to_array(table_t *);

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

table_t *setupTable(Conn *c, int max_clients) {
    struct pollfd *p = (struct pollfd*) malloc(sizeof(struct pollfd));
    table_t *table = new_table(max_clients);

    bzero(p, sizeof(struct pollfd));

    p->fd = c->fd;
    p->events = POLLIN | POLLOUT | POLLHUP;

    
    ms_index = insert(table, p);
    if(ms_index == -1) {
        return NULL;
    }

    return table;
}

// TODO: REWRITE
int poll_table(table_t *t, int timeout_us) {
    // Poll all fds and check which one is ready
    int ready = poll(table_to_array(t), t->nfds, timeout_us);

    // If none are ready then exit
    if(ready == -1 || ms_index == -1) 
        return -1;

    // Assuming all are ready, check for new clients
    if(t->fds[ms_index]->revents & POLLIN) {
        int cfd = accept(t->fds[ms_index]->fd, NULL, NULL);

        // If we cannot accept the new client then err
        if(cfd == -1)
            return -1;

        struct pollfd *tmp = (struct pollfd *) calloc(1, sizeof(struct pollfd));
        tmp->fd = cfd;
        tmp->events = POLLIN | POLLOUT | POLLHUP;

        // // try to add new client
        // if(c->nfds < t->max_cap){
        //     c->fds[c->nfds].fd = cfd;
        //     c->fds[c->nfds].events = POLLIN;
        //     c->nfds += 1;
        // }
        //
        // Warn about capacity overload
        if(insert(t, tmp) < 0) {
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

