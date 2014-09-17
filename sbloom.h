#include "bloom.h"

/*
	SCALABLE BLOOM FILTER
	this works the same as a regular bloom filter,
	but maintains a sequence of bloom filters b_1, ... , b_n
	to check if an element e is in the set we check
	OR(e in b_1, e in b_2, ..., e in b_n)
	We grow a new level when the number of bits set in the last bloom filter
	times growth_factor >= size of the last bloom filter
*/

typedef struct level {
	bfilter_t *bf;
	struct level *next;
} level_t;

typedef struct sbfilter {
	level_t *first;
	level_t *last;
	int num_filters;
	int num_hashes;
	uint32_t size;
	uint32_t growth_factor;
} sbfilter_t;

sbfilter_t *sbf_setup(uint32_t initial_size, int num_hashes, uint32_t growth_factor);
void sbf_teardown(sbfilter_t *sbf);

void sbf_add_member(char *elt, sbfilter_t *sbf);

char sbf_is_member(char *elt, sbfilter_t *sbf);