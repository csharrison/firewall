#define CONFIG_PACKET 0x0
#define DATA_PACKET 0x1

struct data_packet {
    // PAYLOAD
    uint32_t interations;
    uint32_t seed;  

    // HEADER
    uint32_t source;
    uint32_t dest;

    //the number of packets from source to dest making up the entire message between the two
    uint32_t train_size;

    // the packets in a train are ordered by sequence_num, counting from 0 to train_size
    uint32_t sequence_num;

    // a psuedo-random number used to distinguish two concurrent trains between the same source
    // and dest address
    uint32_t tag;
} dpacket_t;

struct config_packet {
    // the update is applied to this address
    uint32_t address;

    // this boolean specifies whetehr <address> is permitted to 
    // send packets (false) or not (true).
    // The operation sets PNG[address] = persona_non_grata
    char persona_non_grata;

    // this half open range of addresses [addressBegin,addressEnd)
    // will either be included in or excluded from 
    // (depending on the setting of acception_range, below)
    // the list of source address intervals previously accepted by address.
    uint32_t address_begin;
    uint32_t address_end;

    // this boolean specifies whether the range of source addresses [address_begin, address_end)
    // should be accepted (true) or rejected (false) by destination address <address>,
    // overriding any previous permissions o that range of source addresses
    char acception_range;
} cpacket_t;

struct pgen {
    // log2(#addresses)
    uint32_t num_addresses_log;
    
    // log2(#num active packet trains)
    uint32_t num_trains_log;
    
    // avg # of packets in a train
    uint32_t mean_train_size;
    
    // avg # of packet trains sent between 2 addresses in any particular communication
    // (a sequence of back-to-back packet trains between the same addresses is a communication)
    uint32_t mean_trains_per_comm;
    
    // avg # of addresses active at any given time. That is, when a new packet train is generated,
    // the source and destination addresses are chosen from a restricted set of active addresses
    uint32_t mean_window;
    
    // avg # of communications that an address is part of (as either source or dest) while active
    // of course, an address can and will become active again, but only after some inactivity
    uint32_t mean_comms_per_address;

    // avg # of <iteration>s in the body of a data packet
    uint32_t mean_work;

    // fraction of packets which are config packets
    float config_fraction;

    // fracion of config packets with persona_non_grata = 1
    float png_fraction
    
    // fraction of config packets with accepting_range = true
    float accepting_fraction;

    // PRIVATE FIELDS - do not set
    uint32_t *_active_addresses;
} pgen_t;

pgen_t *setup_packet_gen(pgen_info_t *);
void tear_down_packet_gen(pgen_t *);
uint32_t get_packet(pgen_t *pgen);