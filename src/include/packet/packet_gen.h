#ifndef PACKETGEN_H
#define PACKETGEN_H

#include "packet/packet.h"
#include "random/random_generator.h"
#include <stdlib.h>
#include <assert.h>
/* private stuff */
typedef struct train {
    addr_t src;
    addr_t dest;
    int train_size;
    int total_trains;
    double mean_work;
    int tag;

    int sequence_number;
    int train_number;
} train_t;
/* /private */


typedef struct pgen {
	double src_residue;
	double dest_residue;
	double speed;
	double exp_mean;
	addr_t src_seed;
	addr_t dest_seed;

    // log2(#addresses)
    int num_addresses_log;

    // log2(#num active packet trains)
    int num_trains_log;

    // avg # of packets in a train
    double mean_train_size;

    // avg # of packet trains sent between 2 addresses in any particular communication
    // (a sequence of back-to-back packet trains between the same addresses is a communication)
    double mean_trains_per_comm;


    double mean_window;

    int mean_comms_per_address;


    // avg # of <iteration>s in the body of a data packet
    double mean_work;

    // fracion of config packets with persona_non_grata = 1
    double png_fraction;

    // fraction of config packets with accepting_range = true
    double accepting_fraction;

    // PRIVATE FIELDS - do not set
    int mask;
    int addresses_mask;
    int time_to_next_config_packet;
    int num_config_packets;
    int config_address_mask;
    train_t *trains;

    addr_t last_config_address;
} pgen_t;

typedef struct pgen_input {
    // log2(#addresses)
    int num_addresses_log;

    // log2(#num active packet trains)
    int num_trains_log;

    // avg # of packets in a train
    double mean_train_size;

    // avg # of packet trains sent between 2 addresses in any particular communication
    // (a sequence of back-to-back packet trains between the same addresses is a communication)
    double mean_trains_per_comm;


    double mean_window;

    int mean_comms_per_address;


    // avg # of <iteration>s in the body of a data packet
    double mean_work;

    double config_fraction;

    // fracion of config packets with persona_non_grata = 1
    double png_fraction;

    // fraction of config packets with accepting_range = true
    double accepting_fraction;
} pgen_input_t;

pgen_t *setup_packet_gen(pgen_input_t *inp);

void tear_down_packet_gen(pgen_t *);
int get_packet(pgen_t *pgen, dpacket_t **data_packet, cpacket_t **config_packet);
#endif