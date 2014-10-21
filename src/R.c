#include "R.h"
/* R */

r_t *r_setup(addr_t size) {
	r_t *r = (r_t *) malloc(sizeof(r_t));

	// initialize as null
	skip_list_t **dests = (skip_list_t **) malloc(size * sizeof(skip_list_t *));
	memset(dest, NULL, size * sizeof(skip_list_t *));

	r->dests = dests;
	r->size = size;

	if (r == NULL || dests == NULL) {
		perror('malloc');
	}

	return r;
}

void r_tear_down(r_t *r) {
	int i;
	for (i = 0 ; i < r->size; i++) {
		if (r->dests[i] != NULL) {
			skip_list_tear_down(r->dests[i]);
		}
	}
	free(r->dests);
	free(r);
}

char r_accept(r_t *r, addr_t dest, addr_t src) {
	skip_list_t *set = r->dests[dest];

	if (set == NULL) {
		return 0;
	}

	return skip_list_contains(set, src);
}

void r_update(r_t *r, char to_add, addr_t dest, addr_t begin, addr_t end) {
	skip_list_t *set = r->dests[dest];

	if (set == NULL) {
		set = skip_list_setup();
		r->dests[dest] = set;
	}

	if (to_add) {
		skip_list_add_range(set, begin, end);
	} else {
		skip_list_remove_range(set, begin, end);
	}
}