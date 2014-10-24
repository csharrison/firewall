#include "collections/queue.h"

squeue_t *squeue_setup(int size) {
	squeue_t *q = malloc(sizeof(squeue_t));
	void **buff = malloc(sizeof(void *) * size);
	q->head = 0;
	q->tail = 0;
	q->size = size;
	q->buff = buff;
	return q;
}

void squeue_tear_down(squeue_t *q) {
	free(q->buff);
	free(q);
}

void squeue_enq(squeue_t *q, void *x) {
	assert(q->tail - q->head < q->size);
	q->buff[q->head % q->size] = x;
	q->tail++;
}

void *squeue_deq() {
	assert(q->tail - q->head > 0);
	void *x = q->buff[q->head % q->size];
	q->head++;
	return x;
}