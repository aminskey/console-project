#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

#include "ipc.h"

#define SOCKET_FILE "/tmp/gamelauncher.sock"


Conn *newConnection(int fd, unsigned int sun_fam, char *sun_path){
    
    Conn *tmp = (Conn *)malloc(sizeof(Conn));
    tmp->fd = fd;
    
    memset(&tmp->saddr, 0, sizeof(struct sockaddr_un));
    tmp->saddr.sun_family = sun_fam;
    memcpy(tmp->saddr.sun_path, sun_path, 128);

    return tmp;
}

int serverConnect(){
    
}
