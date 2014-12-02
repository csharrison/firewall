#include "collections/queue.h"
#include "collections/PNG.h"
#include "collections/R.h"
#include "collections/histogram.h"
#include "packet/packet_gen.h"
#include "throttler.h"
#include <stdatomic.h>
typedef struct reader_info {
	throttler_t *t;

	squeue_t *queue;

	png_t *png;

	r_t *r;

	hist_t *hist;
} reader_info_t;

reader_info_t *reader_setup(throttler_t *t, int queue_size, png_t *png, r_t *r, hist_t *hist);
void reader_tear_down(reader_info_t *r);
void *reader_start(void *wi);
void reader_send_packet(reader_info_t *r, dpacket_t *dp);
