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

// The table structure
typedef struct {
    struct pollfd **fds;
    nfds_t nfds;
    nfds_t max_cap;
} table_t;

// Client array
typedef struct {
    table_t *table;
    uint timeout_us;
} Clients;


// High level API
Conn *serverOpen(char *sock_file, int sock_type);
int serverClose(Conn *c);

// Low-level API
Conn *newConnection(int fd, unsigned int sun_fam, char *sun_path);
table_t *new_table(int size);
void free_table(table_t **t);

int insert(table_t *t, struct pollfd *p);
int search(table_t *t, int fd);
int delete(table_t *t, struct pollfd *p);


#endif
