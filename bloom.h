#include <inttypes.h>

typedef uintmax_t slot_t;

typedef struct bfilter {
	int num_boxes;
	int size;
	int num_hashes; // # hash functions
	slot_t *arr;
} bfilter_t;

bfilter_t *setup(int size, int num_hashes);

void add_member(char *elt, bfilter_t *bf);

char is_member(char *elt, bfilter_t *bf);