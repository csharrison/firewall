#include "types.h"
#define CONFIG_PACKET 0x0
#define DATA_PACKET 0x1

typedef struct data_packet {
    // PAYLOAD
    long interations;
    long seed;

    // HEADER
    addr_t source;
    addr_t dest;

    //the number of packets from source to dest making up the entire message between the two
    int train_size;

    // the packets in a train are ordered by sequence_num, counting from 0 to train_size
    int sequence_num;

    // a psuedo-random number used to distinguish two concurrent trains between the same source
    // and dest address
    int tag;
} dpacket_t;

typedef struct config_packet {
    // this boolean specifies whetehr <address> is permitted to
    // send packets (false) or not (true).
    // The operation sets PNG[address] = persona_non_grata
    char persona_non_grata;

    // this boolean specifies whether the range of source addresses [address_begin, address_end)
    // should be accepted (true) or rejected (false) by destination address <address>,
    // overriding any previous permissions o that range of source addresses
    char accepting_range;

    // the update is applied to this address
    addr_t address;
    // this half open range of addresses [addressBegin,addressEnd)
    // will either be included in or excluded from
    // (depending on the setting of acception_range, below)
    // the list of source address intervals previously accepted by address.
    addr_t address_begin;
    addr_t address_end;
} cpacket_t;

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
uint32_t get_packet(pgen_t *pgen);