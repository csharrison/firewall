#include <math.h>
#define HISTOGRAM_SIZE 16

typedef struct histogram {
    int hist[pow(2,HISTOGRAM_SIZE)] = {0};

} histogram_t;