#include <inttypes.h>

typedef struct bfilter {
	int num_boxes;
	int size;
	int num_hashes; // # hash functions
	uintmax_t *arr;
} bfilter_t;

void set(int index, bfilter_t *bf) {
	int elements_per_box = 1 << (sizeof uintmax_t);
	int box_index = index / elements_per_box;

	// will be [0, intmax (probably 64)]
	int elt_index = index % elements;

	bf->arr[box_index] |= 1 << elt_index;
}

char get(int index, bfilter *bf) {
	int elements_per_box = 1 << (sizeof uintmax_t);
	int box_index = index / elements_per_box;

	// will be [0, intmax (probably 64)]
	int elt_index = index % elements;

	return (bf->arr[box_index] & 1 << elt_index) > 0 ? 1 : 0;
}

bfilter_t *setup(int num_boxes, int num_hashes) {
	bfilter_t *bf = malloc(sizeof(bfilter_t));
	uintmax_t *arr = malloc(sizeof(uintmax_t) * num_boxes);
	memset(arr, 0, sizeof(uintmax_t) * num_boxes);
	
	bf->num_boxes = num_boxes;
	bf->size = num_boxes * sizeof(uintmax_t);
	bf->num_hashes = num_hashes;
	bf->arr = arr;
	return bf;
}

int hash(int fingerprint, int size, int hash_num) {
	return (fingerprint * hash_num) % size;
}

void add(int fingerprint, bfilter_t *bf) {
	int k;
	for (k = 0; k < bf->num_hashes; k++) {
		int index = hash(fingerprint, bf->size, k);
		set(index, bf);
	}
}

char in(int fingerprint, bfilter_t *bf) {
	int k;
	for (k = 0; k < bf->num_hashes; k++) {
		int index = hash(fingerprint, bf->size, k);
		if (! get(index, bf)) return 0;
	}
	return 1;
} 