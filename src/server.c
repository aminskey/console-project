#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <sys/socket.h>
#include <sys/un.h>

#include <ipc.h>

// Define unix domain socket name
#define SOCK_FILE "/tmp/gamelauncher.sock"
#define BUFF_MAX 1024
#define MAX_CLIENTS 10

Conn *serverConn = NULL;

static void handle_sigint(int sig){
    serverClose(serverConn);
    perror("Recieved signal to quit");
    exit(EXIT_SUCCESS);
}

// Entry point
int main(void){
    // Create the server socket and bind!
    serverConn = serverOpen(SOCK_FILE, SOCK_DGRAM);
    signal(SIGINT, handle_sigint);
    signal(SIGABRT, handle_sigint);

    // Validate connection
    if(!serverConn){
        perror("Cannot establish server!");
        return -1;
    }

    // We are running a datagram/UDP server
    // so no need to listen or accept!
    while(1){
        char buff[BUFF_MAX];
        struct sockaddr_un client;
        socklen_t clientLen = sizeof(client);

        memset(buff, '\0', BUFF_MAX);
        ssize_t n = recvfrom(serverConn->fd, &buff, BUFF_MAX - 1, MSG_WAITALL, (struct sockaddr *) &client, &clientLen);
        
        if(n >= 0){
            buff[n] = '\0';
            printf("Recieved from client: %s\n", buff);
        }

    }

    serverClose(serverConn);

    return 0;
}
