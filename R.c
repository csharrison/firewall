#include "R.h"
/* R */

// private methods
pthread_rw_lock_t *_get_lock(r_t *r, addr_t addr);
set_t *_set_setup();
void _set_tear_down(set_t *set);
char _set_accept(set_t *set, addr_t addr);
void _set_add_range(set_t *set, addr_t begin, addr_t end);
void _set_remove_range(set_t *set, addr_t beign, addr_t end);

r_t *setup_r(addr_t size, int log_num_locks) {
	r_t *r = (r_t *) malloc(sizeof(r_t));

	// initialize as null
	set_t *dests = (set_t *) malloc(size * sizeof(set_t*));
	memset(dest, 0, size * sizeof(set_t*));

	pthread_rw_lock_t *locks = (pthread_rw_lock_t *)malloc(r->num_locks * sizeof(pthread_rw_lock_t *));

	r->dests = dests;
	r->size = size;
	r->num_locks = 1 << log_num_locks;
	r->locks = locks;

	if (r == NULL || dests == NULL || locks == NULL) {
		perror('malloc');
	}

	int i;
	for (i = 0; i < r->num_locks; i++) {
		if((pthread_rwlock_init(locks[i], NULL))) {
			perror('pthread_rwlock_init');
			exit(1);
		}
	}

	return r;
}

void tear_down(r_t *r) {
	int i;
	for (i = 0 ; i < r->size; i++) {
		if (r->dests[i]) {
			_set_tear_down(r->dests[i]);
		}
	}
	free(r->dests);
	for(i = 0; i < r->num_locks; i++) {
		pthread_rwlock_destroy(r->locks[i]);
	}
	free(r->locks);
	free(r);
}

char accept(r_t *r, addr_t dest, addr_t src) {
	set_t *set = r->dests[dest];
	pthread_rw_lock_t *lock = _get_lock(r, dest);

	pthread_rwlock_rdlock(lock);
	int accept = set_accept(set, src);
	pthread_rwlock_unlock(lock);

	return accept;
}

void update(r_t *r, char to_add, addr_t dest, addr_t begin, addr_t end) {
	set_t *set = r->dests[dest];
	pthread_rw_lock_t *lock = _get_lock(r, dest);

	pthread_rwlock_wrlock(lock);
	if (to_add) {
		_set_add_range(set, begin, end);
	} else {
		_set_remove_range(set, begin, end);
	}
	pthread_rwlock_unlock(lock);
}

pthread_rw_lock_t *_get_lock(r_t *r, addr_t addr) {
	addr_t stripe_size = r->num_locks / r->size;
	addr_t index = addr / stripe_size;
	return r->locks[index];
}


/* Set */

set_t *_set_setup() {
	set_t* set = malloc(sizeof(set_t));
	return set;
}

void _set_tear_down(set_t *set) {
	free(set);
}

char _set_accept(set_t *set, addr_t addr) {
	return 0;
}

void _set_add_range(set_t *set, addr_t begin, addr_t end) {

}

void _set_remove_range(set_t *set, addr_t beign, addr_t end) {

}