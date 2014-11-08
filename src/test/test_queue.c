#include "test_queue.h"

int test_queues(){
    squeue_t *sq = squeue_setup(MAX_ADDR);
	int *toAdd = 1; 
    squeue_enq(sq, toAdd);
	assert(sq.head != NULL);
	assert(sq.tail != NULL);
	printf("size after enqueue: %d", sq.size);
    squeue_deq();
    printf("size after dequeue: "%d", sq.size);
    int i;
    for (i = 0; i < 100; i++){
    }

    squeue_tear_down(sq);
    return 0;
}
