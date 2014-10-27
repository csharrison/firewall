#include "reader.h"

void _process_dpacket(reader_info_t *w, dpacket_t *packet);
uint16_t _fingerprint(dpacket_t *packet);

reader_info_t *reader_setup(int queue_size, png_t *png, r_t *r, hist_t *hist) {
	reader_info_t *w = malloc(sizeof(reader_info_t));
	w->queue = squeue_setup(queue_size);
	w->png = png;
	w->r = r;
	w->hist = hist;
	return w;
}

void reader_tear_down(reader_info_t *w) {
	squeue_tear_down(w->queue);
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

void reader_send_packet(reader_info_t *r, dpacket_t *dp) {
	squeue_enq(r->queue, (void *)dp);
}

void _process_dpacket(reader_info_t *w, dpacket_t *packet) {
	if (png_allow(w->png, packet->src) &&
		r_accept(w->r, packet->dest, packet->src)) {
		hist_update(w->hist, _fingerprint(packet));
	}
}

uint16_t _fingerprint(dpacket_t *packet) {
	// TODO: fingerprint
	return packet->src;
}