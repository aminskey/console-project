#ifndef _IPC_H_
#define _IPC_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

typedef struct {
    int fd;
    struct sockaddr_un *saddr;
} Conn;

// High level API
Conn *serverOpen(char *sock_file);
int serverClose(Conn *c);

// Low-level API
Conn *newConnection(int fd, unsigned int sun_fam, char *sun_path);
void freeConnection(Conn *c);

#endif
