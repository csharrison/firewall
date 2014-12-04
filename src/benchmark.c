#include "benchmark.h"
#define READERS 2
#define WRITERS 1
#define PACKETS 40000

pgen_input_t p1 = {11, 12, 5 , 1 , 3, 3 , 3822, 0.24, 0.04, 0.96};
pgen_input_t p2 = {12, 10, 1 , 3 , 3, 1 , 2644, 0.11, 0.09, 0.92};
pgen_input_t p3 = {12, 10, 4 , 3 , 6, 2 , 1304, 0.10, 0.03, 0.90};
pgen_input_t p4 = {14, 10, 5 , 5 , 6, 2 , 315 , 0.08, 0.05, 0.90};
pgen_input_t p5 = {15, 14, 9 , 16, 7, 10, 4007, 0.02, 0.10, 0.84};
pgen_input_t p6 = {15, 15, 9 , 10, 9, 9 , 7125, 0.01, 0.20, 0.77};
pgen_input_t p7 = {15, 15, 10, 13, 8, 10, 5328, 0.04, 0.18, 0.80};
pgen_input_t p8 = {16, 14, 15, 12, 9, 5 , 8840, 0.04, 0.19, 0.76};

double time_it(dispatcher_t *d, int serial) {
	struct timespec tb;
	clock_gettime(CLOCK_REALTIME, &tb);
	for (int pnum = 0; pnum < PACKETS; pnum++){
	    dispatch(d, serial);
	}
	struct timespec te;
	clock_gettime(CLOCK_REALTIME, &te);

	long nsecs = te.tv_nsec - tb.tv_nsec;
	time_t secs = te.tv_sec - tb.tv_sec;
	double time_spent = ((double) secs) + ((double)nsecs / 1000000000);
	return time_spent;
}

void collect_data(pgen_input_t *ps, int serial, double *times, int readers, int writers) {
	for (int i = 0; i < 8; i++){
	    pgen_input_t pi = ps[i];
	    pgen_t *pgen = packet_gen_setup(&pi);
	    dispatcher_t *s = dispatcher_setup(pgen, readers, writers);
	    double time_spent = time_it(s, serial);
	    printf("%s: %d. %f\n", serial ? "serial" : "multi ", i, time_spent);
	    times[i] = time_spent;
	    dispatcher_tear_down(s);
	    packet_gen_tear_down(pgen);
	}
}

int main(int argc, char **argv) {
	pgen_input_t ps[] = {p1,p2,p3,p4,p5,p6,p7,p8};
	double serial_times[8] = {0,0,0,0,0,0,0,0};
	double multi_times[8] = {0,0,0,0,0,0,0,0};

    //Running multi-threaded version
    if ((argc > 1 && strcmp(argv[1],"-m") == 0) || (argc == 1)){
    	collect_data(ps, 0, multi_times, READERS, WRITERS);
    }
    if ((argc > 1 && strcmp(argv[1],"-s") == 0) || (argc == 1)){ //only run serial version if benchmark wants to
    	collect_data(ps, 1, serial_times, 0, 0);
    }

    for(int i = 0; i < 8; i++){
    	double st = serial_times[i];
    	double mt = multi_times[i];
    	if (st > 0 && mt > 0) {
    		printf("%f\n", st/mt);
    	}
    }
	return 0;
}
