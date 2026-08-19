#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>

#include <sys/poll.h>

// defining tombstone
struct pollfd tombstone = {
    .events = 0,
    .revents = 0,
    .fd = -1,
};

// The table structure
typedef struct {
    struct pollfd **fds;
    nfds_t nfds;
    nfds_t max_cap;
} table_t;


// Fibonacci hashing
unsigned long hash(int fd) {
    double A = 0.6810339;
    return (unsigned long) floor((double)rand() * ((double)fd*A - floor((double) fd * A)));
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
    int s_index = (int) hash(fd) % (int) t->max_cap;
    int i = s_index;
    
    // TODO: Account for tombstones and NULL
    for(; t->fds[i]->fd != fd; i++){
        if(i == t->max_cap)
            i = 0;
    
        if(i == s_index - 1)
            return -1;
    }

    return i; 
}

int insert(table_t *t, struct pollfd *p) {

    int s_index = (int) hash(p->fd) % (int) t->max_cap;
    int i = s_index;

    for(; t->fds[i] != NULL || t->fds[i]->fd != tombstone.fd; i++){
        if(i == t->max_cap)
            i = 0;

        if(i == s_index - 1)
            return -1;
    }

    t->fds[i] = p;
    return i;
}
