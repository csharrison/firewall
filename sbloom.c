#include "sbloom.h"
#include <stdlib.h>
#include <stdio.h>
level_t *_level_setup(uint32_t size, int num_hashes);

sbfilter_t *sbf_setup(uint32_t initial_size, int num_hashes) {
	sbfilter_t *sbf = malloc(sizeof(sbfilter_t));
	level_t *root = _level_setup(initial_size, num_hashes);

	sbf->first = root;
	sbf->last = root;
	sbf->num_filters = 1;
	sbf->num_hashes = num_hashes;
	sbf->size = initial_size;
	return sbf;
}

void sbf_teardown(sbfilter_t *sbf) {
	level_t *node = sbf->first;
	while (node != NULL) {
		level_t *next = node->next;
		bf_teardown(node->bf);
		free(node);
		node = next;
	}
	free(sbf);
}

void sbf_add_member(char *elt, sbfilter_t *sbf) {
	bf_add_member(elt, sbf->last->bf);
	if (bf_filled(sbf->last->bf, 12)) {
		_add_level(sbf);
	}
}

char sbf_is_member(char *elt, sbfilter_t *sbf) {
	level_t *node = sbf->first;
	while (node != NULL) {
		if (bf_is_member(elt, node->bf)) return 1;
		node = node->next;
	}
	return 0;
}

level_t *_level_setup(uint32_t size, int num_hashes) {
	level_t *level = malloc(sizeof(level_t));
	bfilter_t *bf = bf_setup(size, num_hashes);
	level->bf = bf;
	level->next = NULL;
	return level;
}

level_t *_add_level(sbfilter_t *sbf) {
	level_t *node = sbf->last;
	uint32_t new_size = node->bf->size << 1;
	level_t *child = _level_setup(new_size, node->bf->num_hashes);
	node->next = child;
	sbf->last = child;

	sbf->size += new_size;
	sbf->num_filters++;
	printf("growing to %u\n", sbf->size);
	return child;
}
