#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


/*
	BLOOM FILTER
*/
typedef uintmax_t slot_t;

typedef struct bfilter {
	int num_boxes;
	int num_hashes; // # hash functions
	uint32_t size;
	uint32_t set_bits;
	slot_t *arr;
} bfilter_t;

bfilter_t *bf_setup(uint32_t size, int num_hashes);

void bf_teardown(bfilter_t *bf);

void bf_add_member(char *elt, bfilter_t *bf);

char bf_is_member(char *elt, bfilter_t *bf);

// returns true if the bf is over 1/factor filled
char bf_filled(bfilter_t *bf, uint32_t factor);