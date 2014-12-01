#include "serial.h"
#define MAX_INFLIGHT 256

serial_t *serial_setup(pgen_t *pgen) {
	serial_t *s = (serial_t *)malloc(sizeof(serial_t));

	s->pgen = pgen;
	s->png = png_setup();
	s->r = r_setup(MAX_ADDR);
	s->hist = hist_setup();

	return s;
}

void serial_tear_down(serial_t *s) {
	png_tear_down(s->png);
	r_tear_down(s->r);
	hist_tear_down(s->hist);
	free(s);
}

void dispatch(serial_t *s) {
	cpacket_t *cp = NULL;
	dpacket_t *dp = NULL;

	while (atomic_load(&s->in_flight) == MAX_INFLIGHT) {}
	get_packet(s->pgen, &dp, &cp);

	assert(cp != NULL || dp != NULL);
	if (dp != NULL) {
        //as if a reader processes it
        if (png_allow(s->png, dp->src) && 
            r_accept(s->r, dp->dest, dp->src)){
            hist_update(s->hist, _fingerprint(dp));
        }
	} else {
        //as if a writer processes it
		png_update(s->png, cp->address, cp->persona_non_grata);
        r_update(s->r, cp->accepting_range, cp->address, cp->address_begin,cp->address_end);
	}
}
