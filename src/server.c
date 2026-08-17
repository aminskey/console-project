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
#define TIMEOUT_MS 200

Conn *serverConn;

static void handle_sigint(int sig){
    serverClose(serverConn);
    perror("Recieved signal to quit");
    exit(EXIT_SUCCESS);
}

// Entry point
int main(void){
    // Create the server socket and bind!
    serverConn = serverOpen(SOCK_FILE, SOCK_STREAM);
    Clients *room = newTCPClients(serverConn, MAX_CLIENTS, TIMEOUT_MS);
    signal(SIGINT, handle_sigint);
    signal(SIGABRT, handle_sigint);

    // Validate connection
    if(!serverConn){
        perror("Cannot establish server!");
        return -1;
    }

    // Be prepared to listen
    if(listen(serverConn->fd, MAX_CLIENTS) == -1){
        perror("Cannot listen on socket");
        return -1;
    }

    while(1){
        int ready = pollRoom(room); 
    }

    freeClients(room);
    serverClose(serverConn);

    return 0;
}
