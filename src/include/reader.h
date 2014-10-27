#include "collections/queue.h"
#include "collections/PNG.h"
#include "collections/R.h"
#include "collections/histogram.h"
#include "packet/packet_gen.h"

typedef struct reader_info {
	squeue_t *queue;

	png_t *png;

	r_t *r;

	hist_t *hist;
} reader_info_t;

reader_info_t *reader_setup(squeue_t *queue, png_t *png, r_t *r, hist_t *hist);
void reader_tear_down(reader_info_t *w);
void *reader_start(void *wi);
