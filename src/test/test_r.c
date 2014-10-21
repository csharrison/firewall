#include "test_r.h"

int stress_test_r(r_t *r){
	addr_t i;
	for(i = 0; i < 100; i++) {
		addr_t dest = i;
		addr_t begin = 10;
		addr_t end = 100;
		r_update(r, 1, dest, begin, end);
		r_update(r, 1, dest, begin + 100, end + 100);
	}
	return 0;
}

int test_r() {
	printf("beginning r tests\n");
	r_t *r = r_setup(MAX_ADDR);

	stress_test_r(r);
	printf("done stress testing\n");

	r_tear_down(r);

	printf("ending r tests\n");
	return 0;
}