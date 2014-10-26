#include "collections/PNG.h"
#include <unistd.h>
#include "types.h"
/*PNG*/

png_t *png_setup() {
	png_t *png = (png_t *) malloc(sizeof(png_t));

	// initialize as null
        
	atomic_char dest[1<<MAX_ADDR] = {'0'};

        png->dest = dest;

        if (png == NULL){
                perror("malloc");
        }

	return png;
}

void png_tear_down(png_t *png) {
	free(png);
}

int png_allow(png_t *png, addr_t addr) {
        return atoi(atomic_load(png->dest[addr]));
}

void png_update(png_t *png, addr_t addr, char allow){
        atomic_char val = '0';
        atomic_compare_exchange_strong(png->dest[addr], &val, allow);
}

