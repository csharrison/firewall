#include <time.h>
#include <stdlib.h>

#define MAX_LEVEL 16
#define MAX_ADDR (1<<16)
typedef struct skip_list {
	node_t *head;
	node_t *tail;
} skip_list_t;

/* LOCK FREE LINKED LIST */
typedef struct node {
	char removed;
	char fully_linked;
	addr_t begin;
	addr_t end;
	node_t *next;
	int height;
} list_t;


skip_list_t *skip_list_setup();
void skip_list_tear_down(skip_list_t *sl);

char skip_list_contains(skip_list_t *sl, addr_t addr);
void skip_list_add_range(skip_list_t *sl, addr_t begin, addr_t end);
void skip_list_remove_range(skip_list_t *sl, addr_t begin, addr_t end);