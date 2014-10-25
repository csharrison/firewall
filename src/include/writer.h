#include "collections/queue.h"
#include "collections/PNG.h"
#include "collections/R.h"
#include "packet_gen/packet_gen.h"

typedef struct writer_info {
	squeue_t *queue;

	png_t *png;

	r_t *r;
} writer_info_t;

worker_info_t *writer_setup(squeue_t *queue, png_t *png, r_t *r);
void writer_tear_down(worker_info_t *w);
void *writer_start(void *wi);