#include "throttler.h"

throttler_t *throttler_setup() {
	throttler_t *t = malloc(sizeof(throttler_t));
	pthread_mutex_init(&t->m, NULL);
	t->in_flight = 0;
	return t;
}

void throttler_send(throttler_t *t) {
	pthread_mutex_lock(&t->m);
	t->in_flight++;
	pthread_mutex_unlock(&t->m);
}

void throttler_recieve(throttler_t *t) {
	pthread_mutex_lock(&t->m);
	t->in_flight--;
	pthread_mutex_unlock(&t->m);
}

char throttler_can_send(throttler_t *t, int max) {
	char r;
	pthread_mutex_lock(&t->m);
	r =  t->in_flight < max ? 1 : 0;
	pthread_mutex_unlock(&t->m);
	return r;
}