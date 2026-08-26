#ifndef _IPC_H_
#define _IPC_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <poll.h>
#include <threads.h>

// Structure for a connection
typedef struct {
    int fd;
    struct sockaddr_un *saddr;
} Conn;

// Client array
typedef struct {
    struct pollfd *fds;
    nfds_t nfds;
    nfds_t max_cap;
    uint timeout_us;
} clients_t;


// High level API
Conn *serverOpen(char *sock_file, int sock_type);
int serverClose(Conn *c);
int accept_new_clients(clients_t *t);

// Low-level API
Conn *newConnection(int fd, unsigned int sun_fam, char *sun_path);
clients_t *new_room(Conn *c, nfds_t max_cap, int timeout_us);
void free_room(clients_t **r);

int push(clients_t *r, struct pollfd p);
int pop(clients_t *r);
int remove_client(clients_t *r, int fd);



#endif
