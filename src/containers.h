#include <time.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_LEVEL 16
#define MAX_ADDR (1<<16)
typedef struct skip_list {
	node_t *head;
	node_t *tail;
	pthread_rwlock_t *rwlock;
} skip_list_t;

typedef struct node {
	addr_t begin;
	addr_t end;
	node_t *next;
	int height;
} node_t;


skip_list_t *skip_list_setup();
void skip_list_tear_down(skip_list_t *sl);

char skip_list_contains(skip_list_t *sl, addr_t addr);

/*
invariant: no overlaps before and after any mutating operation
*/
void skip_list_add_range(skip_list_t *sl, addr_t begin, addr_t end);
void skip_list_remove_range(skip_list_t *sl, addr_t begin, addr_t end);