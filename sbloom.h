#include "bloom.h"

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
} sbfilter_t;

sbfilter_t *sbf_setup(uint32_t initial_size, int num_hashes);

void sbf_add_member(char *elt, sbfilter_t *sbf);

char sbf_is_member(char *elt, sbfilter_t *sbf);

// returns the fraction of the bloom filter that is filled
float sbf_filled(sbfilter_t *sbf);