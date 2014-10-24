#include "types.h"
/*PNG*/
typedef struct png {
    int dest[1<<MAX_ADDR] = {0};

} png_t;

//get
int png_get(png_t *png, int addr){
        dest = p.dest;
        return dest[addr];
}

//compareAndSwap
void png_getAndIncrement(png_t *png, int addr){
        
}
