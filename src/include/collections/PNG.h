#ifndef PNG_H
#define PNG_H

#include "collections/skip_list.h"
#include <unistd.h>
#include <stdatomic.h>
#include "types.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*PNG*/
typedef struct png {
        atomic_char *dest;
} png_t;

png_t *png_setup();

void png_tear_down(png_t *png);

int png_allow(png_t *png, addr_t addr);

void png_update(png_t *png, addr_t addr, char allow);

#endif