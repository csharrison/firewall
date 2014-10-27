#include "collections/R.h"
/* R*/

r_t *r_setup(addr_t size) {
	r_t *r = (r_t *) malloc(sizeof(r_t));

	// initialize as null
	atomic_ref *dests = (atomic_ref *) malloc(size * sizeof(atomic_ref));
	int i;
	for (i = 0; i < size; i++) {
		atomic_init(dests + i, NULL);
	}

	r->dests = dests;
	r->size = size;

	if (r == NULL || dests == NULL){
		perror("malloc");
	}

	return r;
}

void r_tear_down(r_t *r) {
	int i;
	for (i = 0 ; i < r->size; i++) {
		atomic_ref *set_ref = r->dests + i;
		skip_list_t *sl = (skip_list_t *)atomic_load(set_ref);
		if (sl != NULL) {
			skip_list_tear_down(sl);
		}
	}
	free(r->dests);
	free(r);
}

skip_list_t *_get_skip_list(r_t *r, addr_t i) {
	return (skip_list_t *)atomic_load(r->dests + i);
}

int r_accept(r_t *r, addr_t dest, addr_t src) {
	skip_list_t *sl = _get_skip_list(r, dest);
	if (sl == NULL) {
		return 0;
	}
	return skip_list_contains(sl, src);
}

void r_update(r_t *r, char to_add, addr_t dest, addr_t begin, addr_t end) {
	skip_list_t *sl = _get_skip_list(r, dest);
	if (sl == NULL) {
		sl = skip_list_setup();
		atomic_ref val = NULL;
		if(! atomic_compare_exchange_strong(r->dests + dest, &val, sl)) {
			skip_list_tear_down(sl);
			sl = _get_skip_list(r, dest);
		}
	}

	if (to_add) {
		skip_list_add_range(sl, begin, end);
	} else {
		skip_list_remove_range(sl, begin, end);
	}
}
