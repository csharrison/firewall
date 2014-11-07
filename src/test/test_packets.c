#include "test_packets.h"

void print_packet(dpacket_t *dp, cpacket_t *cp) {
	if (dp != NULL) {
		printf("D: (%6u -> %6u)\n", dp->src, dp->dest);
	} else if (cp != NULL) {
		printf("config (%u) PNG = %d (%u, %u) (%d)\n", cp->address, cp->persona_non_grata, cp->address_begin, cp->address_end, cp->accepting_range);
	}
}

int test_packets() {
	pgen_input_t pi  = {
		16,
		12,
		5,
		1,
		3,
		3,
		3822,
		.24,
		.04,
		.96
	};

	pgen_t *pgen = packet_gen_setup(&pi);
	for (int i = 0; i < 100; i++) {
		dpacket_t *dp = NULL;
		cpacket_t *cp = NULL;
		get_packet(pgen, &dp, &cp);
		print_packet(dp, cp);
	}

	return 0;
}