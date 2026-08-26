#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>

#include <sys/poll.h>
#include "../headers/ipc.h"

// defining tombstone
struct pollfd tombstone = {
    .events = 0,
    .revents = 0,
    .fd = -1,
};


// Fibonacci hashing
unsigned long hash(int fd) {
    double A = 0.6810339;
    return (unsigned long) floor((double) 6 * ((double)fd*A - floor((double) fd * A)));
}

// Instantiate a new table
table_t *new_table(int size) {
    // Allocate the memory and then zero it
    table_t *tmp = (table_t*) calloc(1, sizeof(table_t));
    // Allocate memory to pollfd pointer and zero it
    tmp->fds = (struct pollfd **) calloc(size, sizeof(struct pollfd *));

    // Assign appropriate values
    tmp->nfds = 0;
    tmp->max_cap = size;

    // return the new table
    return tmp;
}

// killing/freeing old table
void free_table(table_t **t) {
    if(!t || !(*t)){
        errno = EINVAL;
        perror("Argument is null");
        errno = 0;
    }

    // reset all values
    (*t)->nfds = 0;
    (*t)->max_cap = 0;

    // Free all allocated memory
    for(int i = 0; i < (*t)->nfds; i++) free((*t)->fds[i]);
    free((*t)->fds);
    free(*t);

    // seting the actual pointer to NULL
    // This is done via pass by ref
    *t = NULL;
}

int search(table_t *t, int fd) {
    // Starting index
    int s_index = (int) hash(fd) % (int) t->max_cap;
    
    // Iterator index
    int i = s_index;

    // Start searching
    do{
        // Is the slot NULL?
        if(!t->fds[i])
            // If so, return -1;
            return -1;
        
        // Do the file descriptors match?
        if(t->fds[i]->fd == fd)
            // return the found index
            return i;

        // Iterate all slots and wraparound table
        i = (i+1) % t->max_cap;
    // Are we back at where we started
    }while(i != s_index);

    // err since nothing was found
    return -1; 
}

int insert(table_t *t, struct pollfd *p) {

    // Do the pointers exist?
    if(!t || !p)
        return -1;

    // Is there enough space to add a new entry?
    if(t->max_cap == 0 || t->nfds == t->max_cap)
        return -1;

    // Get the start index and set the iterator i
    int s_index = (int) hash(p->fd) % (int) t->max_cap;
    int i = s_index;
   
    // Linear poll the hash table until
    // the statement is false
    while(t->fds[i] && t->fds[i] != &tombstone){
        // Check next slot
        i = (i + 1) % t->max_cap;

        // fail once we wraparound entirely
        // without finding anything
        if(i == s_index)
            return -1;
    }

    // Assign p to empty slot
    t->fds[i] = p;
    t->nfds++;

    // return index of slot
    return i;
}

struct pollfd *table_to_array(table_t *t) {
    struct pollfd *arr = (struct pollfd *)calloc(1, t->nfds);
    
    for(int j = 0, i = 0; i < t->max_cap; i++) {
        if(t->fds[i] && t->fds[i] != &tombstone)
            arr[j++] = *(t->fds[i]);
    }

    return arr;
}

// return index of deleted item or -1
int delete(table_t *t, struct pollfd *p) {
    // Try and find the pollfd via fd
    int i = search(t, p->fd);

    // If it doesn't exist return -1
    if(i == -1)
        return -1;

    // Place a tombstone
    t->fds[i] = &tombstone;
    
    // decrease number of fds
    t->nfds--;

    // return index of deleted item
    return i;
}
