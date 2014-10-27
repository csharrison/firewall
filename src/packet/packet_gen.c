#include "packet/packet_gen.h"

#define SRC 1
#define DEST 2

void _setup_train(pgen_t *pgen, train_t *train);
addr_t _rand_addr(pgen_t *pgen, char type);
int _get_data_packet(pgen_t *pgen, dpacket_t **dp);
int _get_config_packet(pgen_t *pgen, cpacket_t **cp);

pgen_t *setup_packet_gen(
	int num_addresses_log,
	int num_trains_log,
	double mean_train_size,
	double mean_trains_per_comm,
	int mean_window,
	int mean_comms_per_address,
	int mean_work,
	double config_fraction,
	double png_fraction,
	double accepting_fraction) {

    pgen_t *pgen = malloc(sizeof(pgen_t));
    pgen->exp_mean = (1.0 / config_fraction) - 1;
    // from AddressPairGenerator
    pgen->mean_comms_per_address = mean_comms_per_address;
    pgen->num_addresses_log = num_addresses_log;
    pgen->mean_window = (double)mean_window;
    pgen->src_residue = 0.0;
    pgen->dest_residue = 0.0;
    pgen->speed = 2.0 / ((double) mean_comms_per_address);
    pgen->src_seed = 0;
    pgen->dest_seed = 0;


    pgen->mask = (1 << num_trains_log) - 1;
    pgen->addresses_mask = (1 << num_addresses_log) - 1;

    pgen->mean_train_size = mean_train_size;
    pgen->mean_trains_per_comm = mean_trains_per_comm;

    pgen->mean_work = (double) mean_work;

    pgen->config_address_mask = (1 << (num_addresses_log >> 1)) - 1;

    pgen->png_fraction = png_fraction;
    pgen->accepting_fraction = accepting_fraction;

    pgen->trains = (train_t *)malloc(sizeof(train_t) * (unsigned int)(pgen->mask+1));
    for (int i = 0; i <= pgen->mask; i++) {
    	_setup_train(pgen, pgen->trains + i);
    }

    pgen->last_config_address = _rand_addr(pgen, SRC);
    return pgen;
}

int get_packet(pgen_t *pgen, dpacket_t **dp, cpacket_t **cp) {
	if (pgen->time_to_next_config_packet == 0) {
		pgen->num_config_packets++;
		pgen->time_to_next_config_packet = rng_exp(pgen->exp_mean);
		return _get_config_packet(pgen, cp);
	} else {
		return _get_data_packet(pgen, dp);
	}
}

int _get_data_packet(pgen_t *pgen, dpacket_t **dp) {
	if (pgen->time_to_next_config_packet > 0) {
		pgen->time_to_next_config_packet--;
	}
	int train_index = rng_rand() & pgen->mask;

	train_t *train = pgen->trains + train_index;

	dpacket_t *d = malloc(sizeof(dpacket_t));

	// PAYLOAD
	d->iterations = rng_exp(train->mean_work);
	d->seed = rng_rand();

	// HEADER
	d->src = train->src;
	d->dest = train->dest;
	d->train_size = train->train_size;
	d->sequence_number = train->sequence_number;
	d->tag = train->tag;
	*dp = d;


	train->sequence_number++;
	if (train->sequence_number == train->total_trains) {
		_setup_train(pgen, pgen->trains + train_index);
	}

	return 0;
}

int _get_config_packet(pgen_t *pgen, cpacket_t **cp) {
	pgen->last_config_address = _rand_addr(pgen, SRC);
	addr_t addr_begin = (addr_t) rng_uniform_max(pgen->addresses_mask - pgen->config_address_mask);

	cpacket_t *c = malloc(sizeof(cpacket_t));

	c->persona_non_grata = rng_uniform_unit() < pgen->png_fraction ? 1 : 0;
	c->accepting_range = rng_uniform_unit() < pgen->accepting_fraction ? 1 : 0;
	c->address = pgen->last_config_address;
	c->address_begin = addr_begin;
	c->address_end = (addr_t)rng_uniform_range(addr_begin + 1, addr_begin + pgen->config_address_mask);

	assert(c->address_begin < c->address_end);

	*cp = c;
	return 0;
}

addr_t _rand_addr(pgen_t *pgen, char type) {
	double *residue = type == SRC ? &pgen->src_residue : &pgen->dest_residue;
	addr_t *seed = type == SRC ? &pgen->src_seed : &pgen->dest_seed;

	*residue = *residue + pgen->speed * rng_uniform_unit();
	while(*residue > 0.0) {
		int to_add = type == SRC ? 1 : pgen->addresses_mask;
		*seed = (addr_t) ((*seed + to_add) & pgen->addresses_mask);
		*residue -= 1.0;
	}

	int r = rng_mangle(1 + (*seed + rng_exp(pgen->mean_window))) & pgen->addresses_mask;

	return (addr_t)r;
}

void _setup_train(pgen_t *pgen, train_t *train) {
	train->src = _rand_addr(pgen, SRC);
	train->dest = _rand_addr(pgen, DEST);
	train->train_size = rng_exp(pgen->mean_train_size);
	train->total_trains = rng_exp(pgen->mean_trains_per_comm);
	train->mean_work = rng_exp(pgen->mean_work);
	train->tag = rng_rand();
	train->sequence_number = 0;
	train->train_number = 0;
}