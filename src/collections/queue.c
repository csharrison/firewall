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

	pthread_cond_init(&q->cond, NULL);
	pthread_mutex_init(&q->m, NULL);

	return q;
}

void squeue_tear_down(squeue_t *q) {
	pthread_cond_destroy(&q->cond);
	pthread_mutex_destroy(&q->m);
	free(q->buff);
	free(q);
}

void squeue_enq(squeue_t *q, void *x) {
	assert(q->tail - q->head < q->size);

	int t = atomic_load(&q->tail);
	int s = atomic_load(&q->size);

	atomic_store(q->buff + (t % s), (intptr_t) x);

	pthread_mutex_lock(&q->m);

	atomic_fetch_add(&q->tail, 1);
	pthread_cond_signal(&q->cond);

	pthread_mutex_unlock(&q->m);
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
	pthread_mutex_lock(&q->m);
	while(atomic_load(&q->tail) - atomic_load(&q->head) == 0) {
		pthread_cond_wait(&q->cond, &q->m);
	}
	pthread_mutex_unlock(&q->m);

	return squeue_deq(q);
}