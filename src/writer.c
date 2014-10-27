#include "writer.h"

void _process_cpacket(writer_info_t *w, cpacket_t *packet);

writer_info_t *writer_setup(squeue_t *queue, png_t *png, r_t *r) {
	writer_info_t *w = malloc(sizeof(writer_info_t));
	w->queue = queue;
	w->png = png;
	w->r = r;
	return w;
}

void writer_tear_down(writer_info_t *w) {
	free(w);
}

void *writer_start(void *wi) {
	writer_info_t *w = (writer_info_t *)wi;

	while (1) {
		cpacket_t *packet = (cpacket_t *)squeue_deq_wait(w->queue);
		_process_cpacket(w, packet);
	}
	return NULL;
}

void _process_cpacket(writer_info_t *w, cpacket_t *p) {

	png_update(w->png, p->address, p->persona_non_grata);
	r_update(w->r, p->accepting_range, p->address, p->address_begin, p->address_end);
}
