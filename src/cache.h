#ifndef CACHE_H

#define CACHE_H

#include <time.h>

#include "process_file.h"

typedef struct node_t {
    char *target;
    file_cont_t *content;
    struct node_t *next;
    struct node_t *prev;
    time_t created_at;
} node_t;

typedef struct {
    struct node_t *head;  /* holds the head of the doubly linked list */
    struct node_t *tail;  /* holds the tail of the doubly linked list */
    int capacity;         /* holds max number of allowed entries */
    int size;             /* holds size of list */ 
    int expr;             /* after how long to expire entries */
} cache_t;

cache_t *create_cache(int, int);
node_t *create_node_t(char *, file_cont_t *);
void delete_cache(cache_t **);
void free_node_t(node_t **);
void insert_to_cache(cache_t *, char *, file_cont_t *);
void remove_from_cache(cache_t *);

#endif
