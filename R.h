
typdef addr_t uint16_t;

/* set represented as a skip list of address ranges */
typedef struct set {

} set_t;


/* R */

typedef struct r {
	addr_t size;
	set_t *dests;
	int num_locks;
	pthread_rwlock_t *locks;
} r_t;

r_t *setup_r(addr_t size, int log_num_locks);

void tear_down(r_t *r);

char accept(r_t *r, addr_t dest, addr_t src);

void update(r_t *r, addr_t dest, addr_t begin, addr_t end);

