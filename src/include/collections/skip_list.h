#ifndef SKIPLIST_H
#define SKIPLIST_H
#include "types.h"
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>

#define MAX_LEVEL 16

typedef struct node {
	addr_t begin;
	addr_t end;
	int height;
	struct node **next;
} node_t;

typedef struct skip_list {
	node_t *head;
	node_t *tail;
	pthread_rwlock_t *rwlock;
} skip_list_t;



skip_list_t *skip_list_setup();
void skip_list_tear_down(skip_list_t *sl);

int skip_list_contains(skip_list_t *sl, addr_t addr);

/*
invariant: no overlaps before and after any mutating operation
*/
int skip_list_add_range(skip_list_t *sl, addr_t begin, addr_t end);
void skip_list_remove_range(skip_list_t *sl, addr_t begin, addr_t end);

void skip_list_print(skip_list_t *sl);

#endif