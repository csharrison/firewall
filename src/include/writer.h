#include "collections/queue.h"
#include "collections/PNG.h"
#include "collections/R.h"
#include "packet/packet_gen.h"
#include <stdatomic.h>

typedef struct writer_info {
	atomic_int *in_flight;

	squeue_t *queue;

	png_t *png;

	r_t *r;
} writer_info_t;

writer_info_t *writer_setup(atomic_int *in_flight, int queue_size, png_t *png, r_t *r);
void writer_tear_down(writer_info_t *w);
void writer_send_packet(writer_info_t *w, cpacket_t *cp);
void *writer_start(void *wi);