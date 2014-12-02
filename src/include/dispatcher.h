#include "collections/queue.h"
#include "collections/PNG.h"
#include "collections/R.h"
#include "packet/packet_gen.h"
#include "writer.h"
#include "reader.h"
#include <stdio.h>
#include <stdatomic.h>

typedef struct dispatcher {
	int num_readers;
	int num_writers;

	throttler_t *t;

	pgen_t *pgen;
	png_t *png;
	r_t *r;
	hist_t *hist;
	pthread_t *treaders;
	pthread_t *twriters;
	reader_info_t **ireaders;
	writer_info_t **iwriters;
} dispatcher_t;

dispatcher_t *dispatcher_setup(pgen_t *pgen, int num_readers, int num_writers);

void dispatcher_tear_down(dispatcher_t *d);

void dispatch(dispatcher_t *d);

dispatcher_t *serial_setup(pgen_t *pgen);

void serial_tear_down(dispatcher_t *d);

void serial_dispatch(dispatcher_t *d);
