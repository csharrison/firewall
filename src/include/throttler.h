#ifndef THROTTLE_H
#define THROTTLE_H

#include <pthread.h>

#include <stdlib.h>

typedef struct throttler {
	pthread_mutex_t m;
	int in_flight;
} throttler_t;

throttler_t *throttler_setup();
void throttler_send(throttler_t *t);
void throttler_recieve(throttler_t *t);
char throttler_can_send(throttler_t *t, int max);

#endif