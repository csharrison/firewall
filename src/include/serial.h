#include "collections/PNG.h"
#include "collections/R.h"
#include "collections/histogram.h"
#include "packet/packet_gen.h"
#include <stdio.h>
#include <stdatomic.h>

typedef struct serial {
	pgen_t *pgen;
	png_t *png;
	r_t *r;
	hist_t *hist;
} serial_t;

serial_t *serial_setup(pgen_t *pgen);

void serial_tear_down(serial_t *d);

void serial_dispatch(serial_t *d);
