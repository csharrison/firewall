#include "packet/packet.h"

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
    // log2(#addresses)
    uint32_t num_addresses_log;

    // log2(#num active packet trains)
    uint32_t num_trains_log;

    // avg # of packets in a train
    double mean_train_size;

    // avg # of packet trains sent between 2 addresses in any particular communication
    // (a sequence of back-to-back packet trains between the same addresses is a communication)
    double mean_trains_per_comm;

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
} pgen_t;

pgen_t *setup_packet_gen();
void tear_down_packet_gen(pgen_t *);
uint32_t get_packet(pgen_t *pgen, dpacket_t **data_packet, cpacket_t **config_packet);