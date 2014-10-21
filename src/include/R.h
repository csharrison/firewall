#include "containers.h"
#include "stdatomic.h"
/* R */

typedef atomic_intptr_t atomic_ref;

typedef struct r {
	addr_t size;
	atomic_ref *dests; //stores skip_list_t's
} r_t;


r_t *r_setup(addr_t size);

void r_tear_down(r_t *r);

int r_accept(r_t *r, addr_t dest, addr_t src);

void r_update(r_t *r, char to_add, addr_t dest, addr_t begin, addr_t end);
