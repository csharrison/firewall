#include "collections/queue.h"

squeue_t *squeue_setup(int size) {
	squeue_t *q = malloc(sizeof(squeue_t));
	void **buff = malloc(sizeof(void *) * size);

	pthread_mutex_init(&q->empty_m, NULL);
	pthread_cond_init(&q->empty_cv, NULL);

	q->head = 0;
	q->tail = 0;
	q->size = size;
	q->buff = buff;
	return q;
}

void squeue_tear_down(squeue_t *q) {
	pthread_mutex_destroy(&q->empty_m);
	pthread_cond_destroy(&q->empty_cv);
	free(q->buff);
	free(q);
}

void squeue_enq(squeue_t *q, void *x) {
	assert(q->tail - q->head < q->size);
	q->buff[q->head % q->size] = x;
	q->tail++;

	pthread_mutex_lock(&q->empty_m);
	pthread_cond_signal(&q->empty_cv);
	pthread_mutex_unlock(&q->empty_m);
}

void *squeue_deq(squeue_t *q) {
	assert(q->tail - q->head > 0);
	void *x = q->buff[q->head % q->size];
	q->head++;
	return x;
}

void *squeue_deq_wait(squeue_t *q) {
	if (q->tail - q->head > 0) return squeue_deq(q);

	pthread_mutex_lock(&q->empty_m);
	while(q->tail - q->head == 0) {
		pthread_cond_wait(&q->empty_cv, &q->empty_m);
	}
	pthread_mutex_unlock(&q->empty_m);

	return squeue_deq(q);
}