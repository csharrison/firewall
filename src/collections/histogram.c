#include "types.h"
/*histogram*/
typedef struct h {
    int hist[1<<MAX_ADDR] = {0};

} h_t;

//get
int histogram_get(h_t *h, int addr){
        hist = h.hist;
        return hist[addr];
}

//getAndIncrement
void histogram_getAndIncrement(h_t *h, int addr){
        
}
