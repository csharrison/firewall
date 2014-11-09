#include "collections/queue.h"

squeue_t *squeue_setup(int size) {
	squeue_t *q = malloc(sizeof(squeue_t));
	atomic_intptr_t *buff = malloc(sizeof(atomic_intptr_t) * (unsigned int)size);

	q->head = ATOMIC_VAR_INIT(0);
	q->tail = ATOMIC_VAR_INIT(0);
	q->size = size;
	q->buff = buff;

	assert(q->size >0);
	return q;
}

void squeue_tear_down(squeue_t *q) {
	free(q->buff);
	free(q);
}

void squeue_enq(squeue_t *q, void *x) {
	assert(q->size > 0);
	assert(q->tail - q->head < q->size);

	int h = atomic_load(&q->head);

	atomic_store(q->buff + (h % q->size), (atomic_intptr_t) x);

	atomic_fetch_add(&q->tail, 1);
}

void *squeue_deq(squeue_t *q) {
	assert(q->size > 0);
	assert(atomic_load(&q->tail) - atomic_load(&q->head) > 0);

	int h = atomic_load(&q->head);

	void *x = (void *) atomic_load(q->buff + (h % q->size));

	atomic_fetch_add(&q->head, 1);
	return x;
}

void *squeue_deq_wait(squeue_t *q) {
	if (atomic_load(&q->tail) - atomic_load(&q->head)) return squeue_deq(q);

	while(atomic_load(&q->tail) - atomic_load(&q->head) == 0) {
		// spin spin spin spin
	}

	return squeue_deq(q);
}