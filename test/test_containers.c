#include "containers.h"

typedef struct range {
	addr_t begin;
	addr_t end;
} range_t;

typedef struct test_sl {
	skip_list_t *sl;
	range_t *ranges;
	int index;
} test_sl_t;

int includes_range(skip_list_t *sl, addr_t begin, addr_t end) {
	addr_t addr;
	for(addr = begin; addr < end; addr++) {
		if(!skip_list_contains(sl, addr)) {
			return 0;
		}
	}
	return 1;
}

int add_range(test_sl_t *test, addr_t begin, addr_t end) {
	skip_list_add_range(test->sl, begin, end);
	int i = test->index
	test->ranges[i]->begin = begin;
	test->ranges[i]->end = end;

	test->index = i + 1;
}

int remove_range(test_sl_t *test, addr_t begin, addr_t end) {
	skip_list_remove_range(test->sl, begin, end);
	int i = test->index;
	test->ranges[i]->begin = begin;
	test->ranges[i]->end = end;

	test->index = i + 1;
}

int main(int argc, char **argv) {

}