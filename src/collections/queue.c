#include "collections/queue.h"

squeue_t *squeue_setup(int size) {
	squeue_t *q = malloc(sizeof(squeue_t));
	q->buff = malloc(sizeof(atomic_intptr_t) * (unsigned int)size);

	for(int i = 0; i < size; i++) {
		q->buff[i] = ATOMIC_VAR_INIT(0);
	}

	q->head = ATOMIC_VAR_INIT(0);
	q->tail = ATOMIC_VAR_INIT(0);
	q->size = ATOMIC_VAR_INIT(size);

	return q;
}

void squeue_tear_down(squeue_t *q) {
	free(q->buff);
	free(q);
}

void squeue_enq(squeue_t *q, void *x) {
	assert(q->tail - q->head < q->size);

	int t = atomic_load(&q->tail);
	int s = atomic_load(&q->size);

	atomic_store(q->buff + (t % s), (intptr_t) x);

	atomic_fetch_add(&q->tail, 1);
}

void *squeue_deq(squeue_t *q) {
	assert(atomic_load(&q->tail) - atomic_load(&q->head) > 0);

	int h = atomic_load(&q->head);
	int s = atomic_load(&q->size);

	intptr_t x =  atomic_load(q->buff + (h % s));

	atomic_fetch_add(&q->head, 1);
	return (void *)x;
}

void *squeue_deq_wait(squeue_t *q) {
	if (atomic_load(&q->tail) - atomic_load(&q->head)) return squeue_deq(q);

	while(atomic_load(&q->tail) - atomic_load(&q->head) == 0) {
		// spin spin spin spin
	}

	return squeue_deq(q);
}