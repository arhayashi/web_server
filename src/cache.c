/*
 * This file contains
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cache.h"

/*
 * This function creates the cache, taking in the maximum number of allowed
 * entries and the amount of time after which entries should expire.
 * It holds the head of the doubly linked list which holds the node_t
 * entries.
 */

cache_t *create_cache(int capacity, int expr) {
    cache_t *cache = malloc(sizeof(cache_t));

    if (cache == NULL) {
        return NULL;
    }

    cache->head = NULL;
    cache->tail = NULL;
    cache->capacity = capacity;
    cache->size = 0;
    cache->expr = expr;

    return cache;
} /* create_cache() */

/*
 * This function creates a node_t entry to be used in the cache. It takes the
 * requested target, and a pointer to the file's content.
 */

node_t *create_node_t(char *target, file_cont_t *content) {
    node_t *node = malloc(sizeof(node_t));

    if (node == NULL) {
        return NULL;
    }

    node->target = NULL;
    node->content = NULL;
    node->next = NULL;
    node->prev = NULL;
    node->created_at = 0;

    node->target = malloc(strlen(target) + 1);

    /* create space for target and copy it into node */

    if (node->target == NULL) {
        free(node);
        return NULL;
    }

    strcpy(node->target, target);

    node->content = content;
    node->created_at = 1;  /* change */

    return node;
} /* create_node_t() */

/*
 * This function frees the given node_t entry.
 */

void free_node_t(node_t **node) {
    if ((node == NULL) || (*node == NULL)) {
        return;
    }

    // FRee file_cont_t ???

    free((*node)->target);
    free(*node);
    *node = NULL;
} /* delete_node_t() */

/*
 * This function deletes a cache and all of it's node_t entries.
 */

void delete_cache(cache_t **cache) {
    if ((cache == NULL) || (*cache == NULL)) {
        return;
    }

    while ((*cache)->head != NULL) {
        node_t *temp_node = (*cache)->head;
        (*cache)->head = temp_node->next;
        free_node_t(&temp_node);
    }

    (*cache)->tail = NULL;

    free(*cache);
    *cache = NULL;
} /* delete_cache() */

/*
 * This function inserts a node_t node to the beginning of the cache.
 */

void insert_to_cache(cache_t *cache, char *target, file_cont_t *content) {
    printf("[LOG] adding target %s to cache\n", target);

    /* remove last entry if too many entries */

    if (cache->size >= cache->capacity) {
        remove_from_cache(cache);
    }

    node_t *new_node = create_node_t(target, content);

    if (new_node == NULL) {
        fprintf(stderr, "[WARNING] unable to add requested target to cache\n");
        return;
    }

    node_t *head_node = cache->head;

    if (head_node != NULL) {
        head_node->prev = new_node;
    } else {
        cache->tail = new_node;
    }

    new_node->next = head_node;

    cache->head = new_node;

    cache->size += 1;
} /* insert_to_cache() */

/*
 * This function removes the last entry from the cache.
 */

void remove_from_cache(cache_t *cache) {
    node_t *tail_node = cache->tail;

    if (tail_node == NULL) {
        return;
    }

    if (tail_node->prev != NULL) {
        tail_node->prev->next = tail_node->next;
    }

    cache->tail = tail_node->prev;

    free_node_t(&tail_node);

    cache->size -=1;
} /* remove_from_cache() */

# if 0

int main(void) {
    cache_t *cache = create_cache(10, 0);
    node_t *p;

    printf("inserting...\n");

    insert_to_cache(cache, "/1", NULL);
    insert_to_cache(cache, "/2", NULL);
    insert_to_cache(cache, "/3", NULL);

    p = cache->head;

    while (p) {
        printf("target: %s\n", p->target);
        p = p->next;
    }

    printf("removing...\n");

    remove_from_cache(cache);

    p = cache->head;

    while (p) {
        printf("target: %s\n", p->target);
        p = p->next;
    }

    delete_cache(&cache);
}

#endif
