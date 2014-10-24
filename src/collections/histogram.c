#include "types.h"
#include "stdatomic.h"
/*histogram*/

typedef struct h {
    atomic_int hist[1<<MAX_ADDR] = {0};

} h_t;

//get
int histogram_get(h_t *h, int addr){
        return (int)atomic_load(h->hist[addr]);
}

//getAndIncrement
void histogram_getAndIncrement(h_t *h, int addr){
        atomic_fetch_add(h->hist[addr],1);     
}
