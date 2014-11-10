#include "collections/PNG.h"
/*PNG*/

png_t *png_setup() {
	png_t *png = (png_t *) malloc(sizeof(png_t));
	atomic_char *dest = (atomic_char *)malloc(sizeof(atomic_char) * MAX_ADDR);
	png->dest = dest;
	memset(png->dest, 0, sizeof(atomic_char) * MAX_ADDR);

	if (png == NULL || dest == NULL){
		perror("malloc");
    }

	return png;
}

void png_tear_down(png_t *png) {
	free(png->dest);
	free(png);
}

int png_allow(png_t *png, addr_t addr) {
	return atomic_load(png->dest +addr);
}

void png_update(png_t *png, addr_t addr, char allow){
	atomic_char val = 0;
	atomic_compare_exchange_strong(png->dest + addr, &val, &allow);
}

