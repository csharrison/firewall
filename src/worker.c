#include "reader.h"

void _process_dpacket(reader_info_t *w, dpacket_t *packet);
uint16_t _fingerprint(dpacket_t *packet);

reader_info_t *reader_setup(squeue_t *queue, png_t *png, r_t *r, hist_t *hist) {
	reader_info_t *w = malloc(sizeof(reader_info_t));
	w->queue = queue;
	w->png = png;
	w->r = r;
	w->hist = hist;
	return w;
}

void reader_tear_down(reader_info_t *w) {
	free(w);
}

void *reader_start(void *wi) {
	reader_info_t *w = (reader_info_t *)wi;

	while (1) {
		dpacket_t *packet = (dpacket_t *)squeue_deq_wait(w->queue);
		_process_dpacket(w, packet);
	}
	return NULL;
}

void _process_dpacket(reader_info_t *w, dpacket_t *packet) {
	if (png_allow(w->png, packet->source) &&
		r_accept(w->r, packet->dest, packet->source)) {
		hist_add(w->hist, fingerprint(packet));
	}
}

uint16_t _fingerprint(dpacket_t *packet) {
	// TODO: fingerprint
	return packet->source;
}