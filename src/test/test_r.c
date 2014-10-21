#include "test_r.h"

void *add_tons_r(void *vr) {
	r_t *r = (r_t *)vr;
	addr_t i;
	for(i = 0; i < 100; i++) {
		int j;
		for (j = 0; j < 100; j++) {
			addr_t dest = i;
			addr_t begin = j * 10;
			addr_t end = j * 10 + 5;
			r_update(r, 1, dest, begin, end);
		}
	}
	return 0;
}

int stress_test_r(r_t *r, int n) {
	pthread_t threads[n];
	int t;
	for (t = 0; t < n; t++) {
		pthread_create(&threads[t], NULL, add_tons_r, (void *)r);
	}
	for (t = 0; t < n; t++) {
		pthread_join(threads[t], NULL);
	}
	return 0;
}

int test_r() {
	printf("beginning r tests\n");
	r_t *r = r_setup(MAX_ADDR);

	stress_test_r(r, 20);
	printf("done stress testing\n");

	r_tear_down(r);

	printf("ending r tests\n");
	return 0;
}