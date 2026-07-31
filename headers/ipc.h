#ifndef _IPC_H_
#define _IPC_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

typedef struct {
    int fd;
    struct sockaddr_un saddr;
} Conn;

#endif
