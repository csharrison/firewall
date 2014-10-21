#include "test_containers.h"

int includes_range(skip_list_t *sl, addr_t begin, addr_t end, addr_t expected) {
	addr_t addr;
	addr_t count = 0;
	for(addr = begin; addr < end; addr++) {
		if(skip_list_contains(sl, addr)) {
			count++;
		} else {
			printf("miss at addr %u\n", addr);
		}
	}
	if (count != expected) {
		printf("ERROR <includes_range>: expected count %d, got %d\n", expected, count);
	}
	return count;
}

void *add_tons(void *vsl) {
	skip_list_t *sl = (skip_list_t *)vsl;
	int i;
	for (i = 0; i < 100; i++) {
		addr_t b = (addr_t) rand();
		addr_t e = (addr_t) (b + 10);
		// protect overflows
		if (b < e) skip_list_add_range(sl, b, e);
	}
	return NULL;
}

int stress_test(skip_list_t *sl, int n) {
	pthread_t threads[n];
	int t;
	for (t = 0; t < n; t++) {
		pthread_create(&threads[t], NULL, add_tons, (void *)sl);
	}
	for (t = 0; t < n; t++) {
		pthread_join(threads[t], NULL);
	}
	skip_list_print(sl);
	return 0;
}

int test_containers() {
	addr_t ranges[10];
	ranges[0] = 1;
	ranges[1] = 5;

	ranges[2] = 4;
	ranges[3] = 7;

	ranges[4] = 12;
	ranges[5] = 14;

	ranges[6] = 100;
	ranges[7] = 200;

	ranges[8] = 50;
	ranges[9] = 101;

	skip_list_t *sl = skip_list_setup();
	int i;
	for (i = 0; i < 10; i = i + 2) {
		addr_t b = (addr_t) ranges[i];
		addr_t e = (addr_t) ranges[i+1];

		addr_t expect = (addr_t)(e - b);
		skip_list_add_range(sl, b, e);
		skip_list_print(sl);
		includes_range(sl, b, e, expect);
	}

	skip_list_remove_range(sl, 1, 7);
	skip_list_print(sl);
	skip_list_remove_range(sl, 75, 150);
	skip_list_print(sl);
	skip_list_remove_range(sl, 1, 250);
	skip_list_print(sl);
	skip_list_remove_range(sl, 0, MAX_ADDR);
	printf("killed all\n");
	skip_list_print(sl);
	stress_test(sl, 200);
	skip_list_remove_range(sl, 0, MAX_ADDR);
	skip_list_print(sl);
	skip_list_tear_down(sl);
	return 0;
}