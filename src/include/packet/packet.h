#include "types.h"
#define CONFIG_PACKET 0x0
#define DATA_PACKET 0x1

typedef struct data_packet {
    // PAYLOAD
    long interations;
    long seed;

    // HEADER
    addr_t src;
    addr_t dest;

    //the number of packets from source to dest making up the entire message between the two
    int train_size;

    // the packets in a train are ordered by sequence_num, counting from 0 to train_size
    int sequence_number;

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