#include "reader.h"

void _process_dpacket(reader_info_t *w, dpacket_t *packet);
uint16_t _fingerprint(dpacket_t *packet);

reader_info_t *reader_setup(atomic_int *in_flight, int queue_size, png_t *png, r_t *r, hist_t *hist) {
	reader_info_t *w = malloc(sizeof(reader_info_t));
	w->in_flight = in_flight;
	w->queue = squeue_setup(queue_size);
	w->png = png;
	w->r = r;
	w->hist = hist;
	return w;
}

void reader_tear_down(reader_info_t *r) {
	squeue_tear_down(r->queue);
	free(r);
}

void *reader_start(void *ri) {
	reader_info_t *r = (reader_info_t *)ri;

	while (1) {
		dpacket_t *packet = (dpacket_t *)squeue_deq_wait(r->queue);
		if (packet == NULL) break;
		_process_dpacket(r, packet);

		atomic_fetch_sub(r->in_flight, 1);
		free(packet);
	}
	return NULL;
}

void reader_send_packet(reader_info_t *r, dpacket_t *dp) {
	squeue_enq(r->queue, (void *)dp);
}

void _process_dpacket(reader_info_t *r, dpacket_t *packet) {
	if (png_allow(r->png, packet->src) &&
		r_accept(r->r, packet->dest, packet->src)) {
		hist_update(r->hist, _fingerprint(packet));
	}
}

uint16_t _fingerprint(dpacket_t *packet) {
	// TODO: fingerprint
	return packet->src;
}