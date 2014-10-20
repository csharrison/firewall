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

int stress_test(skip_list_t *sl, int n) {
	int i;
	for (i = 0; i < n; i++) {
		addr_t b = (addr_t) rand();
		addr_t e = b + 10;
		skip_list_add_range(sl, b, e);
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

		addr_t expect = e - b;
		skip_list_add_range(sl, b, e);
		skip_list_print(sl);
		includes_range(sl, b, e, expect);
	}
	stress_test(sl, 100);
	skip_list_tear_down(sl);
	return 0;
}