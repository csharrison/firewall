#include "writer.h"

void _process_cpacket(worker_info_t *w, cpacket_t *packet);

worker_info_t *writer_setup(squeue_t *queue, png_t *png, r_t *r) {
	worker_info_t *w = malloc(sizeof(worker_info_t));
	w->queue = queue;
	w->png = png;
	w->r = r;
	return w;
}

void writer_tear_down(worker_info_t *w) {
	free(w);
}

void *writer_start(void *wi) {
	worker_info_t *w = (worker_info_t *)wi;

	while (1) {
		cpacket_t *packet = (cpacket_t *)squeue_deq_wait(w->queue);
		_process_cpacket(w, packet);
	}
	return NULL;
}

void _process_cpacket(worker_info_t *w, cpacket_t *p) {
	png_set(w->png, p->persona_non_grata);
	r_update(w->r, p->accepting_range, p->address, p->address_begin, p->address_end);
}
