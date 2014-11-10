#include "benchmark.h"
#define READERS 1
#define WRITERS 1
#define PACKETS 62165

pgen_input_t p1 = {11, 12, 5 , 1 , 3, 3 , 3822, 0.24, 0.04, 0.96};
pgen_input_t p2 = {12, 10, 1 , 3 , 3, 1 , 2644, 0.11, 0.09, 0.92};
pgen_input_t p3 = {12, 10, 4 , 3 , 6, 2 , 1304, 0.10, 0.03, 0.90};
pgen_input_t p4 = {14, 10, 5 , 5 , 6, 2 , 315 , 0.08, 0.05, 0.90};
pgen_input_t p5 = {15, 14, 9 , 16, 7, 10, 4007, 0.02, 0.10, 0.84};
pgen_input_t p6 = {15, 15, 9 , 10, 9, 9 , 7125, 0.01, 0.20, 0.77};
pgen_input_t p7 = {15, 15, 10, 13, 8, 10, 5328, 0.04, 0.18, 0.80};
pgen_input_t p8 = {16, 14, 15, 12, 9, 5 , 8840, 0.04, 0.19, 0.76};

int main() {
	pgen_input_t ps[] = {p1,p2,p3,p4,p5,p6,p7,p8};
	for(int i = 0; i < 8; i++) {
		pgen_input_t pi = ps[i];
		pgen_t *pgen = packet_gen_setup(&pi);
		dispatcher_t *d = dispatcher_setup(pgen, READERS, WRITERS);

		clock_t begin = clock();
		for (int pnum = 0; pnum < PACKETS; pnum++) {
			dispatch(d);
		}
		double time_spent = (double)(clock() - begin) / CLOCKS_PER_SEC;
		printf("Parameter %d -> %f\n", i, time_spent);

		dispatcher_tear_down(d);
		packet_gen_tear_down(pgen);

	}
	return 0;
}