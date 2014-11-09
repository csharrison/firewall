#ifndef SQUEUE_H
#define SQUEUE_H

#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <stdatomic.h>
/*
	A simple wait-free single enqueuer - single dequeuer queue
	note: this cannot store more than `size` elements
*/
typedef struct squeue {
	int size;
	atomic_int head;
	atomic_int tail;
	atomic_intptr_t *buff;
} squeue_t;

squeue_t *squeue_setup(int size);
void squeue_tear_down(squeue_t *q);
void squeue_enq(squeue_t *q, void *x);
void *squeue_deq();

// waits
void *squeue_deq_wait(squeue_t *q);

#endif