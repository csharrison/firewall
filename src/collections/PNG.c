#include "collections/PNG.h"
/*PNG*/

png_t *png_setup() {
	png_t *png = (png_t *) malloc(sizeof(png_t));
	atomic_char *dest = (atomic_char *)malloc(sizeof(atomic_char) * MAX_ADDR);
	png->dest = dest;

	if (png == NULL || dest == NULL){
		perror("malloc");
    }
	char allow = 0;
	for(int i = 0; i < MAX_ADDR; i++) {
		atomic_store(png->dest + i, allow);
	}
	return png;
}

void png_tear_down(png_t *png) {
	free(png->dest);
	free(png);
}

int png_allow(png_t *png, addr_t addr) {
	char persona_non_grata = atomic_load(png->dest + addr);
	assert(persona_non_grata == 0 || persona_non_grata == 1);
	return persona_non_grata == 0 ? 1 : 0;
}

void png_update(png_t *png, addr_t addr, char allow){
	assert(allow == 0 || allow == 1);
	atomic_char val = allow == 0 ? 1 : 0;
	atomic_compare_exchange_strong(png->dest + addr, &val, allow);
}

