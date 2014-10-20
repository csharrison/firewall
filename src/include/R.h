#include "containers.h"

/* R */

typedef struct r {
	addr_t size;
	skip_list_t **sl;
} r_t;

r_t *r_setup(addr_t size);

void r_tear_down(r_t *r);

char r_accept(r_t *r, addr_t dest, addr_t src);

void r_update(r_t *r, char to_add, addr_t dest, addr_t begin, addr_t end);
