#include "writer.h"

void _process_cpacket(writer_info_t *w, cpacket_t *packet);

writer_info_t *writer_setup(atomic_int *in_flight, int queue_size, png_t *png, r_t *r) {
	writer_info_t *w = malloc(sizeof(writer_info_t));
	w->in_flight = in_flight;
	w->queue = squeue_setup(queue_size);
	w->png = png;
	w->r = r;
	return w;
}

void writer_tear_down(writer_info_t *w) {
	squeue_tear_down(w->queue);
	free(w);
}

void *writer_start(void *wi) {
	writer_info_t *w = (writer_info_t *)wi;

	while (1) {
		cpacket_t *packet = (cpacket_t *)squeue_deq_wait(w->queue);
		if (packet == NULL) break;
		_process_cpacket(w, packet);
		atomic_fetch_sub(w->in_flight, 1);
		free(packet);
	}
	return NULL;
}

void writer_send_packet(writer_info_t *w, cpacket_t *cp) {
	squeue_enq(w->queue, (void *)cp);
}

void _process_cpacket(writer_info_t *w, cpacket_t *p) {

	png_update(w->png, p->address, p->persona_non_grata);
	r_update(w->r, p->accepting_range, p->address, p->address_begin, p->address_end);
}
