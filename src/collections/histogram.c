#include "types.h"

typedef struct histogram {
    int hist[1<<MAX_ADDR] = {0};

} histogram_t;
