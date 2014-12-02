#include "collections/queue.h"

squeue_t *squeue_setup(int size) {
	squeue_t *q = malloc(sizeof(squeue_t));
	q->buff = malloc(sizeof(void *) * (unsigned int)size);

	for(int i = 0; i < size; i++) {
		q->buff[i] = 0;
	}

	q->head = (0);
	q->tail = (0);
	q->size = (size);

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

	int t = q->tail;
	int s = q->size;

	q->buff[t %s] = x;

	pthread_mutex_lock(&q->m);

	q->tail++;
	if(q->tail == q->head + 1) {
		pthread_cond_signal(&q->cond);
	}

	pthread_mutex_unlock(&q->m);
}

void *squeue_deq(squeue_t *q) {
	assert(q->tail - q->head > 0);
	void *x = q->buff[q->head % q->size];

	q->head++;
	return x;
}

void *squeue_deq_wait(squeue_t *q) {

	pthread_mutex_lock(&q->m);
	while(q->tail == q->head) {
		pthread_cond_wait(&q->cond, &q->m);
	}
	pthread_mutex_unlock(&q->m);

	return squeue_deq(q);
}