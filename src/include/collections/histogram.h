#include "types.h"
#include "stdatomic.h"
/*histogram*/

typedef struct hist {
    atomic_long dest[MAX_ADDR];

} hist_t;

hist_t *hist_setup();

void hist_tear_down(hist_t *hist);

void hist_update(hist_t *hist, uint16_t fingerprint);
