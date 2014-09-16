#include <inttypes.h>

typedef uintmax_t slot_t;

typedef struct bfilter {
	int num_boxes;
	int size;
	int num_hashes; // # hash functions
	slot_t *arr;
} bfilter_t;

bfilter_t *bf_setup(int size, int num_hashes);

void bf_add_member(char *elt, bfilter_t *bf);

char bf_is_member(char *elt, bfilter_t *bf);

void bf_stat(bfilter_t *bf);