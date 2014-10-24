#include
/*
	A simple wait-free single enqueuer - single dequeuer queue
	note: this cannot store more than `size` elements
*/
struct squeue {
	int head;
	int tail;
	int size;
	void **buff;

	// condition variable for broadcasting
	// on the waiting dequeuer
	pthread_cond_t empty_cv;
	pthread_mutex_t empty_m;
} squeue_t;

squeue_t *squeue_setup(int size);
void squeue_tear_down(squeue_t *q);
void squeue_enq(squeue_t *q, void *x);
void *squeue_deq();

// waits on a condition variable
void *squeue_deq_wait(squeue_t *q);