#ifndef _IPC_H_
#define _IPC_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <poll.h>


typedef struct {
    int fd;
    struct sockaddr_un *saddr;
} Conn;

typedef struct {
    int max_clients;
    int timeout;
    nfds_t nfds;
    struct pollfd *fds;
} Clients;

// High level API
Conn *serverOpen(char *sock_file, int sock_type);
int serverClose(Conn *c);

// Low-level API
Conn *newConnection(int fd, unsigned int sun_fam, char *sun_path);
Clients *newTCPClients(Conn *c, int max_clients, int timeout_us);

void freeConnection(Conn *c);
void freeClients(Clients *c);

#endif
