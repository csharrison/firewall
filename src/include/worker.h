#include "collections/queue.h"
#include "collections/PNG.h"
#include "collections/R.h"
#include "packet_gen/packet_gen.h"
typedef struct worker_info {
	squeue_t *queue;

	png_t *png;

	r_t *r;

	hist_c *hist;
} worker_info_t;