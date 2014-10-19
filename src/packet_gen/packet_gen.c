#include "packet_gen.h"

typedef struct train {
	uint32_t num_packets;

} train_t;

typedef struct comm {
	uint32_t num_trains;
	train_t *current_train;	
} comm_t;

typedef struct active_addr {
	uint32_t num_coms;
	comm_t *current_comm;
} active_addr_t;

pgen_t *setup_packet_gen(info *pgen_t){
    pgen_t *pgen = malloc(sizeof(pgen_t));
    memcpy(pgen, info, sizeof(pgen_t));

    return pgen;
}

uint32_t get_packet(pgen_t *pgen) {

}

uint32_t get_data_packet(pgen_t *pgen) {

}

uint32_t get_config_packet(pgen_t *pgen) {

}