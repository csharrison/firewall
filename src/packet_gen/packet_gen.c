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

pgen_t *setup_packet_gen(
	int num_addresses_log,
	int num_trains_log,
	double mean_train_size,
	double mean_trains_per_comm;
	int mean_window,
	int mean_comms_per_address,
	int mean_work,
	double config_fraction,
	double png_fraction,
	double accepting_fraction) {

    pgen_t *pgen = malloc(sizeof(pgen_t));
    pgen->num_addresses_log = num_addresses_log;
    pgen->num_trains_log = num_trains_log;
    pgen->mean_train_size = mean_train_size;
    pgen->mean_trains_per_comm = mean_trains_per_comm;
    pgen->mean_work = (double)mean_work;


    return pgen;
}

uint32_t get_packet(pgen_t *pgen) {
	if (pgen->time_to_next_config_packet == 0) {
		pgen->num_config_packets++;
		pgen->time_to_next_config_packet = rng_exp(pgen->exp_mean);
		return get_config_packet();
	} else {
		return get_data_packet();
	}
}

uint32_t get_data_packet(pgen_t *pgen) {
	if (pgen->time_to_next_config_packet >0) {
		pgen->time_to_next_config_packet--;
	}
	int train_index = rng_rand() & pgen->mask

}

uint32_t get_config_packet(pgen_t *pgen) {

}