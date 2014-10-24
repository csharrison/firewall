
void *start_worker(void *wi) {
	worker_info_t *w = (worker_info_t *)wi;

	while (1) {
		dpacket_t *packet = (dpacket_t *)squeue_deq_wait(w->queue);
		process_packet(w, packet);
	}
	return NULL;
}

void process_packet(worker_info_t *w, dpacket_t *packet) {
	if (png_allow(w->png, packet->source) &&
		r_accept(w->r, packet->dest, packet->source)) {
		hist_add(w->hist, fingerprint(packet));
	}
}

uint16_t fingerprint(dpacket_t *packet) {
	// TODO: fingerprint
	return packet->source;
}