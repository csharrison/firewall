#include "bloom.h"

#include <stdlib.h>
#include <string.h>

void _set(uint32_t index, bfilter_t *bf);
char _get(uint32_t index, bfilter_t *bf);
uint32_t _fingerprint(char *str);
uint32_t _hash(uint32_t fingerprint, uint32_t size, uint32_t hash_num);

bfilter_t *bf_setup(int size, int num_hashes) {
	int min_slots = size / (sizeof(slot_t));
	uint32_t num_boxes = 1;
	while(num_boxes < min_slots) num_boxes = num_boxes << 1;

	bfilter_t *bf = (bfilter_t *)malloc(sizeof(bfilter_t));
	slot_t *arr = (slot_t *)malloc(sizeof(slot_t) * num_boxes);
	memset(arr, 0, sizeof(slot_t) * num_boxes);

	bf->num_boxes = num_boxes;
	bf->size = num_boxes * sizeof(slot_t);
	bf->num_hashes = num_hashes;
	bf->arr = arr;
	return bf;
}

void bf_add_member(char *elt, bfilter_t *bf) {
	uint32_t fprint = _fingerprint(elt);
	int k;
	for (k = 0; k < bf->num_hashes; k++) {
		int index = _hash(fprint, bf->size, k);
		_set(index, bf);
	}
}

char bf_is_member(char *elt, bfilter_t *bf) {
	uint32_t fprint = _fingerprint(elt);
	int k;
	for (k = 0; k < bf->num_hashes; k++) {
		int index = _hash(fprint, bf->size, k);
		if (! _get(index, bf)) return 0;
	}
	return 1;
}

void _set(uint32_t index, bfilter_t *bf) {
	uint32_t box_index = index / sizeof(slot_t);
	uint32_t elt_index = index % sizeof(slot_t);

	bf->arr[box_index] |= 1 << elt_index;
}

char _get(uint32_t index, bfilter_t *bf) {
	uint32_t box_index = index / sizeof(slot_t);
	uint32_t elt_index = index % sizeof(slot_t);

	return (bf->arr[box_index] & 1 << elt_index) > 0 ? 1 : 0;
}


uint32_t _fingerprint(char *str) {
	uint32_t fp = 5381;
	char c;
	while ((c = *str++)) {
		fp = ((fp << 5) + fp) ^ c;
	}
	return fp;
}

uint32_t _hash(uint32_t fingerprint, uint32_t size, uint32_t hash_num) {
	return (fingerprint * hash_num) % size;
}

