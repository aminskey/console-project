#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>

#include <sys/poll.h>
#include "../headers/ipc.h"

struct pollfd ignore = {
    .fd = -1,
    .events = 0,
    .revents = 0
};

clients_t *new_room(Conn *c, nfds_t max_cap, int timeout_us) {
    clients_t *room = (clients_t *) calloc(1, sizeof(clients_t));
    room->fds = (struct pollfd *) calloc(max_cap, sizeof(struct pollfd));
    
    room->fds[0].fd = c->fd;
    room->fds[0].events = POLLIN | POLLHUP;

    room->nfds = 1;
    room->timeout_us = timeout_us;
    room->max_cap = max_cap;

    return room;
}

int push(clients_t *room, struct pollfd p) {
    if(!room || room->max_cap == room->nfds)
        return -1;

    room->fds[room->nfds] = p;
    room->nfds++;
    return 1;
}

int pop(clients_t *room) {
    if(!room)
        return -1;
    // We do this to ensure that the actual data
    // is deleted and not just forgotten
    room->fds[room->nfds] = ignore;
    room->nfds--;

    return 1;
}

int remove_client(clients_t *room, int fd) {

    if(!room || fd < 0) {
        return -1;
    }

    for(int i = 0; i < room->nfds; i++) {
        if(room->fds[i].fd == fd) {
            // replacing the empty slot with
            // the latest client, since order
            // doesn't matter right now
            if(i < room->nfds - 1)
                room->fds[i] = room->fds[room->nfds];
            return pop(room);
        }
    }

    return -1;
}

void free_room(clients_t **r) {
    free((*r)->fds);
    
    (*r)->fds = NULL;
    (*r)->nfds = 0;
    (*r)->timeout_us = 0;
    (*r)->max_cap = 0;

    free(*r);
    r = NULL;
}
