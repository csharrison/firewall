#include "types.h"
#include "stdatomic.h"
/*PNG*/
typedef struct png {
    atomic_int dest[1<<MAX_ADDR] = {0};

} png_t;

//get
int png_get(png_t *png, int addr){
        return (int)atomic_load(png->dest[addr]);
}

//compareAndSwap
bool png_getAndIncrement(png_t *png, int addr, atomic_int expected, atomic_int swap){
        return atomic_compare_exchange_strong(png->dest[addr],expected,swap); 
}
