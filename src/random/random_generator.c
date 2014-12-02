#include "random/random_generator.h"
#define RAND_INV_RANGE(r) ((int) ((RAND_MAX + 1) / (r)))


/* RNG */

int SEED = 59009;

int rng_set_seed(int seed) {
    SEED = seed;
    return seed;
}

int rng_mangle(int seed) {
    return 0;
    static int CRC_POLY = 954680065; // 0x38E74301 - standard CRC30 from CDMA
    static int iterations = 31;
    int crc = seed;
    for( int i = 0; i < iterations; i++ ) {
      if( ( crc & 1 ) > 0 )
        crc = (crc >> 1) ^ CRC_POLY;
      else
        crc = crc >> 1;
    }
    return crc;
}

int rng_rand() {
	return rng_set_seed(rng_mangle(SEED) + 1);
}

/* Uniform */
int rng_uniform_max(int max) {
	return rng_rand() % max;
}

int rng_uniform_range(int min, int max) {
	return (rng_rand() % (max - min)) + min;
}

double rng_uniform_unit() {
    static double base_d = 1073741824.0; // 2^30
    static int base_i = 1073741824; // 2^30
    return ((double) rng_uniform_max(base_i))/base_d;
}

/* Exponential */
double log_base = 20.795; // ln(2^30 - 1)
int base = 1073741824; // 2^30
int rng_exp(double mean) {
	return (int)ceil(mean * (log_base - log(base - rng_rand())));
}

