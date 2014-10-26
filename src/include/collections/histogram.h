#include "types.h"
<<<<<<< HEAD
#include "stdatomic.h"
/*histogram*/

typedef struct h {
    atomic_long dest[1<<MAX_ADDR];

} h_t;

hist_t *hist_setup();

void hist_tear_down(hist_t *hist);

void hist_update(hist_t *hist, uint16_t fingerprint);
=======
void hist_add(hist_t *h, addr_t addr);
>>>>>>> 2c1e2a28886b6ceea0c0b9145325f62d98f8ea89
