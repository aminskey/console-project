#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <signal.h>

#include <asm/termbits.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/poll.h>
#include <sys/ioctl.h>

#include <ipc.h>
#include "../headers/ipc.h"

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

static void handle_sigsegv(int sig) {
    serverClose(serverConn);
    printf("Failed due to SIGSEGV\n");
    printf("Please check for\n");
    printf("\t * dereferenced null pointers\n");
    printf("\t * uninitialised memory\n");
    printf("\t * non-existing memory that is being accessed\n");
    exit(EXIT_FAILURE);
}

// Entry point
int main(void){
    // Create the server socket and bind!
    serverConn = serverOpen(SOCK_FILE, SOCK_STREAM);
    clients_t *room = new_room(serverConn, MAX_CLIENTS, TIMEOUT_MS);
    int ms_index = 0;

    // Setting up handling for different signals
    signal(SIGINT, handle_sigint);
    signal(SIGABRT, handle_sigint);
    signal(SIGSEGV, handle_sigsegv);

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
        int ready = accept_new_clients(room);
        
        if(ready == -1) break; 
        if(ready == 0) continue;


        for(int i = 0; i < room->nfds; i++){
            if(room->fds[i].revents & POLLIN){
                char buff[BUFF_MAX];
                bzero(buff, BUFF_MAX);

                int len = 0;
                ioctl(room->fds[i].fd, FIONREAD, &len);

                if(len > 0){                
                    if(read(room->fds[i].fd, buff, len) == -1) {
                        perror("Cannot read from socket :(\n");
                    } else {
                        printf("> From client.fd(%d): %s\n", room->fds[i].fd, buff);
                        room->fds[i].events |= POLLOUT;
                    }
                }
            }

            if(room->fds[i].revents & POLLOUT) {
                char *msg = "Hello from server!";
                write(room->fds[i].fd, msg, strlen(msg));
                room->fds[i].events &= ~POLLOUT;
            }

            if(room->fds[i].revents & POLLHUP) {
                printf(">Client.fd(%d) tried to hang up\n", room->fds[i].fd);
                remove_client(room, room->fds[i].fd);
            }
        }
    }

    free_room(&room);
    serverClose(serverConn);

    return 0;
}
