#ifndef __KAD_PACKET_STRUCT__
#define __KAD_PACKET_STRUCT__

#include "libed2k/packet_struct.hpp"

namespace libed2k {


// MOD Note: end
#define EDONKEYVERSION                  0x3C
#define KADEMLIA_VERSION1_46c           0x01 /*45b - 46c*/
#define KADEMLIA_VERSION2_47a           0x02 /*47a*/
#define KADEMLIA_VERSION3_47b           0x03 /*47b*/
#define KADEMLIA_VERSION5_48a           0x05 // -0.48a
#define KADEMLIA_VERSION6_49aBETA       0x06 // -0.49aBETA1, needs to support: OP_FWCHECKUDPREQ (!), obfuscation, direct callbacks, source type 6, UDP firewallcheck
#define KADEMLIA_VERSION7_49a           0x07 // -0.49a needs to support OP_KAD_FWTCPCHECK_ACK, KADEMLIA_FIREWALLED2_REQ
#define KADEMLIA_VERSION8_49b           0x08 // TAG_KADMISCOPTIONS, KADEMLIA2_HELLO_RES_ACK
#define KADEMLIA_VERSION9_50a           0x09 // handling AICH hashes on keyword storage
#define KADEMLIA_VERSION                KADEMLIA_VERSION5_48a // Change CT_EMULE_MISCOPTIONS2 if Kadversion becomes >= 15 (0x0F)


#define KADEMLIA_FIND_VALUE     0x02
#define KADEMLIA_STORE          0x04
#define KADEMLIA_FIND_NODE      0x0B
#define KADEMLIA_FIND_VALUE_MORE    KADEMLIA_FIND_NODE

#define KADEMLIA_TOLERANCE_ZONE 24

//#define KADEMLIA_VERSION    0x08    /* 0.49b */

    // COMMON KAD packages
    /**
      * KAD identifier compatible with eMule format serialization
      *
    */
    struct kad_id : public md4_hash {

        enum {
            kad_total_bits = md4_hash::size * 8
        };

        kad_id();
        kad_id(const md4_hash& h);
        kad_id(const md4_hash::md4hash_container&);

        template<typename Archive>
        void save(Archive& ar) {
            for (unsigned i = 0; i < sizeof(m_hash); ++i) {
                ar & m_hash[(i / 4) * 4 + 3 - (i % 4)];
                //uint32_t value = (((uint32_t)m_hash[i] << 24) | ((uint32_t)m_hash[i+1] << 16) | ((uint32_t)m_hash[i+2] << 8) | ((uint32_t)m_hash[i+3]));
                //ar & value;
            }
        }

        template<typename Archive>
        void load(Archive& ar) {
            for (unsigned i = 0; i < sizeof(m_hash); ++i) {
                uint8_t c;
                ar & c;
                m_hash[(i / 4)*4 + 3 - (i % 4)] = c;
            }
        }

        LIBED2K_SERIALIZATION_SPLIT_MEMBER()
    };

    struct kad_net_identifier {
        client_id_type  address;
        uint16_t        udp_port;
        uint16_t        tcp_port;

        template<typename Archive>
        void serialize(Archive& ar) {
            ar & address & udp_port & tcp_port;
        }
    };

    struct kad_entry {
        kad_id              kid;
        kad_net_identifier  address;
        uint8_t             version;

        template<typename Archive>
        void serialize(Archive& ar) {
            ar & kid & address & version;
        }
    };

    struct kad2_common_res {
        kad_id      kid;
        uint16_t    tcp_port;
        uint8_t     version;

        template<typename Archive>
        void serialize(Archive& ar) {
            ar & kid & tcp_port & version;
        }
    };

    struct kad_info_entry {
        md4_hash        hash;
        tag_list<uint8_t> tags;

        template<typename Archive>
        void serialize(Archive& ar) {
            ar & hash & tags;
        }
    };

    // BOOTSTRAP packages
    struct kad_bootstrap_req {
        kad_id  kid;
        kad_net_identifier  address;
        uint8_t tail;

        template<typename Archive>
        void serialize(Archive& ar) {
            ar & kid & address & tail;
        }
    };

    /**
     * this is common kad information reply in bootstrap and kademlia2_res
     * will use in observer reply call also
     */
    typedef std::deque<kad_entry>   kad_contacts_res;

    typedef container_holder<uint16_t, kad_contacts_res >   kad_bootstrap_res;

    struct kad2_bootstrap_req {

        template<typename Archive>
        void serialize(Archive& ar) {}
    };

    struct kad2_bootstrap_res {
        kad2_common_res client_info;
        container_holder<uint16_t, kad_contacts_res > contacts;

        template<typename Archive>
        void serialize(Archive& ar) {
            ar & client_info & contacts;
        }
    };

    // HELLO packages
    struct kad_hello_base {
        kad_id  kid;
        kad_net_identifier  address;
        uint8_t tail;

        template<typename Archive>
        void serialize(Archive& ar) {
            ar & kid & address & tail;
        }
    };

    struct kad_hello_req : public kad_hello_base {};
    struct kad_hello_res : public kad_hello_base {};

    struct kad2_hello_base {
        kad2_common_res     client_info;
        tag_list<uint8_t>   tags;

        template<typename Archive>
        void serialize(Archive& ar) {
            ar & client_info & tags;
        }
    };

    struct kad2_hello_req : public kad2_hello_base {};
    struct kad2_hello_res : public kad2_hello_base {};

    // SEARCH sources packages
    struct kademlia_req {
        uint8_t search_type;
        kad_id  kid_target;
        kad_id  kid_receiver;

        template<typename Archive>
        void serialize(Archive& ar) {
            ar & search_type & kid_target & kid_receiver;
        }
    };

    struct kademlia_res {
        kad_id  kid_target;
        container_holder<uint8_t, kad_contacts_res >    results;
        template<typename Archive>
        void serialize(Archive& ar) {
            ar & kid_target & results;
        }
    };

    struct kademlia2_req : public kademlia_req {};
    struct kademlia2_res : public kademlia_res {};

    struct kad_search_req {
        kad_id  target_id;

    };

    struct kad_search_res {
        kad_id  target_id;
        container_holder<uint16_t, std::deque<kad_info_entry> > results;

        template<typename Archive>
        void searialize(Archive& ar) {
            ar & target_id & results;
        }
    };

    struct kad2_search_res {
        kad_id  source_id;
        kad_id  target_id;
        container_holder<uint16_t, std::deque<kad_info_entry> > results;

        template<typename Archive>
        void serialize(Archive& ar) {
            ar & source_id & target_id & results;
        }
    };

    struct kad_search_notes_req {
        kad_id  target_id;

        template<typename Archive>
        void serialize(Archive& ar) {
            ar & target_id;
        }
    };

    struct kad_search_notes_res {
        kad_id  note_id;
        container_holder<uint16_t, std::deque<kad_info_entry> > notes;
    };

    struct kad2_search_sources_req {
        kad_id  target_id;
        uint16_t    start_position;
        uint64_t    size;

        template<typename Archive>
        void serialize(Archive& ar) {
            ar & target_id & start_position & size;
            start_position &= 0x7FFF;   // temp code
        }
    };

    struct kad2_search_key_req {
        kad_id  target_id;
        uint16_t start_position;

        template<typename Archive>
        void serialize(Archive& ar) {
            ar & target_id & start_position;
        }
    };

    struct kad2_search_notes_req {
        kad_id      target_id;
        uint64_t    filesize;

        template<typename Archive>
        void serialize(Archive& ar) {
            ar & target_id & filesize;
        }
    };

    // PUBLISH packages

    struct kad2_publish_key_req {
        kad_id  client_id;
        container_holder<uint16_t, std::deque<kad_info_entry> >  keys;

        template<typename Archive>
        void serialize(Archive& ar) {
            ar & client_id & keys;
        }
    };

    struct kad2_publish_source_req {
        kad_id  client_id;
        kad_id  source_id;
        tag_list<uint8_t>   tags;

        template<typename Archive>
        void serialize(Archive& ar) {
            ar & client_id & source_id & tags;
        }
    };


    struct kad_publish_note_req {
        kad_id  note_id;
        kad_id  publisher_id;
        tag_list<uint8_t>   tags;

        template<typename Archive>
        void serialize(Archive& ar) {
            ar & note_id & publisher_id & tags;
        }
    };

    struct kad2_publish_res {
        kad_id  target_id;
        uint8_t count;

        template<typename Archive>
        void serialize(Archive& ar) {
            ar & target_id; // &count;
            // read last field with catch exception - possibly isn't exist
        }
    };


    struct kad2_publish_key_res : public kad2_publish_res {};
    struct kad2_publish_source_res : public kad2_publish_res {};
    struct kad2_publish_note_res : public kad2_publish_res {};


    // FIREWALLED

    struct kad_firewalled_req {
        uint16_t    tcp_port;

        template<typename Archive>
        void serialize(Archive& ar) {
            ar & tcp_port;
        }
    };

    struct kad2_firewalled_req {
        uint16_t    tcp_port;
        kad_id      user_id;

        template<typename Archive>
        void serialize(Archive& ar) {
            ar & tcp_port & user_id;
        }
    };

    struct kad_firewalled_res {
        client_id_type ip;

        template<typename Archive>
        void serialize(Archive& ar) {
            ar & ip;
        }
    };

    struct kad_firewalled_ack_res {
        template<typename Archive>
        void serialize(Archive& ar) {}
    };

    struct kad2_firewalled_up {
        uint8_t error_code;
        uint16_t    incoming_port;

        template<typename Archive>
        void serialize(Archive& ar) {
            ar & error_code & incoming_port;
        }
    };


    // PING+PONG

    struct kad2_ping {
        template<typename Archive>
        void serialize(Archive& ar) {
        }
    };

    struct kad2_pong {
        uint16_t    udp_port;
        template<typename Archive>
        void serialize(Archive& ar) {
            ar & udp_port;
        }
    };

    /*
    struct kad_answer_base{
        md4_hash    kad_id;
        md4_hash    key_id;
    };

    struct kad_bootstrap_contact{
        md4_hash        client_id;
        client_id_type  client_ip;
        boost::uint16_t udp_port;
        boost::uint16_t tcp_port;
        boost::uint8_t  version;
    };

    struct kad_booststrap_req {
        template<typename Archive>
        void serialize(Archive& ar) {}
    };

    struct kad_booststrap_res{
        md4_hash    kad_id;
        boost::uint16_t port;
        boost::uint8_t  kad_version;
        container_holder<boost::uint16_t, std::deque<kad_booststrap_req> >  contacts;
        template<typename Archive>
        void serialize(Archive& ar) {
            ar & kad_id & port & kad_version & contacts;
        }
    };


    struct kad_hello_req{
        md4_hash    client_id;
        boost::uint16_t port;
        boost::uint8_t  version;
        bool            udp_firewalled; // unused
        bool            tcp_firewalled; // unused
        tag_list<boost::uint8_t>    options;
        template<typename Archive>
        void serialize(Archive& ar) {
            ar & client_id & port & version & options;
        }
    };

    struct kad_hello_res{
        md4_hash    client_id;
        boost::uint16_t port;
        boost::uint8_t  version;
        tag_list<boost::uint8_t>    options;    // external kad port and udp/tcp firewalled
        template<typename Archive>
        void serialize(Archive& ar) {
            ar & client_id & port & version & options;
        }
    };

    struct kad_hello_res_ack{
        md4_hash    client_id;
        template<typename Archive>
        void serialize(Archive& ar) {
            ar & client_id;
        }
    };

    struct kad_search_response{
        kad_answer_base header;
        md4_hash        answer;
        container_holder<boost::uint16_t, tag_list<boost::uint8_t> >  tags;    // tag list doesn't support container's interface
        template<typename Archive>
        void serialize(Archive& ar) {
            ar & header & answer & tags;
        }
    };

    struct kad_find_buddy_req{
        md4_hash    target_id;
        md4_hash    client_id;
        boost::uint16_t port;
    };

    struct kad_find_buddy_res{
        md4_hash    target_id;
        md4_hash    client_id;
        boost::uint16_t port;
    };

    // find source case
    struct kad_callback_req{
        md4_hash    target_id;
        md4_hash    file_id;
        boost::uint16_t port;
    };
    */

    struct kad_entry_ext {
        uint32_t    dw_key;
        uint32_t    dw_ip;
        uint8_t     verified;

        template<typename Archive>
        void serialize(Archive& ar) {
            ar & dw_key & dw_ip & verified;
        }
    };

    /**
      * this structure describes standard emule nodes.dat file internal structure
    */
    struct kad_nodes_dat {
        uint32_t    num_contacts;
        uint32_t    version;
        uint32_t    bootstrap_edition;
        container_holder<uint32_t, std::deque<kad_entry> >   bootstrap_container;
        std::list<kad_entry>        contacts_container;
        std::list<kad_entry_ext>    ext_container;

        template<typename Archive>
        void save(Archive& ar) {
            LIBED2K_ASSERT(false);
        }

        template<typename Archive>
        void load(Archive& ar) {
            ar & num_contacts;
            if (num_contacts == 0) {
                ar & version;
                if (version == 3) {
                    ar & bootstrap_edition;
                    if (bootstrap_edition == 1) {
                        ar & bootstrap_container;
                        return;
                    }
                }

                if (version >= 1 && version <= 3) {
                    ar & num_contacts;
                }
            }

            for (size_t i = 0; i != num_contacts; ++i) {
                kad_entry ke;
                kad_entry_ext kee;
                ar & ke;
                contacts_container.push_back(ke);
                if (version >= 2) {
                    ar & kee;
                    ext_container.push_back(kee);
                }
            }
        }

        LIBED2K_SERIALIZATION_SPLIT_MEMBER()
    };

    template<> struct packet_type<sources_request>{
        static const proto_type value       = OP_REQUESTSOURCES;
        static const proto_type protocol    = OP_EMULEPROT;
    };
    template<> struct packet_type<sources_request2>{
        static const proto_type value       = OP_REQUESTSOURCES2;
        static const proto_type protocol    = OP_EMULEPROT;
    };
    template<> struct packet_type<sources_answer>{
        static const proto_type value       = OP_ANSWERSOURCES;
        static const proto_type protocol    = OP_EMULEPROT;
    };
    template<> struct packet_type<sources_answer2>{
        static const proto_type value       = OP_ANSWERSOURCES2;
        static const proto_type protocol    = OP_EMULEPROT;
    };

    template<> struct packet_type<kad2_ping> {
        static const proto_type value = KADEMLIA2_PING;
        static const proto_type protocol = OP_KADEMLIAHEADER;
    };

    template<> struct packet_type<kad2_pong> {
        static const proto_type value = KADEMLIA2_PONG;
        static const proto_type protocol = OP_KADEMLIAHEADER;
    };

    template<> struct packet_type<kad2_hello_req> {
        static const proto_type value = KADEMLIA2_HELLO_REQ;
        static const proto_type protocol = OP_KADEMLIAHEADER;
    };

    template<> struct packet_type<kad2_hello_res> {
        static const proto_type value = KADEMLIA2_HELLO_RES;
        static const proto_type protocol = OP_KADEMLIAHEADER;
    };

    template<> struct packet_type<kad2_bootstrap_req> {
        static const proto_type value = KADEMLIA2_BOOTSTRAP_REQ;
        static const proto_type protocol = OP_KADEMLIAHEADER;
    };

    template<> struct packet_type<kad2_bootstrap_res> {
        static const proto_type value = KADEMLIA2_BOOTSTRAP_RES;
        static const proto_type protocol = OP_KADEMLIAHEADER;
    };

    template<> struct packet_type<kad_firewalled_req> {
        static const proto_type value = KADEMLIA_FIREWALLED_REQ;
        static const proto_type protocol = OP_KADEMLIAHEADER;
    };

    template<> struct packet_type<kad_firewalled_res> {
        static const proto_type value = KADEMLIA_FIREWALLED_RES;
        static const proto_type protocol = OP_KADEMLIAHEADER;
    };

    template<> struct packet_type<kademlia2_req> {
        static const proto_type value = KADEMLIA2_REQ;
        static const proto_type protocol = OP_KADEMLIAHEADER;
    };

    template<> struct packet_type<kademlia2_res> {
        static const proto_type value = KADEMLIA2_RES;
        static const proto_type protocol = OP_KADEMLIAHEADER;
    };

    /**
    *   special transaction identifier on packet type
    */
    template<typename T> struct transaction_identifier;

    template<> struct transaction_identifier<kad2_ping> { static const uint16_t id = 112; };
    template<> struct transaction_identifier<kad2_pong> { static const uint16_t id = 112; };

    // bootstrap request/response identifiers
    template<> struct transaction_identifier<kad2_bootstrap_req> {  static const uint16_t id = 98; };
    template<> struct transaction_identifier<kad2_bootstrap_res> {  static const uint16_t id = 98; };

    // hello request/response
    template<> struct transaction_identifier<kad2_hello_req> { static const uint16_t id = 104; };
    template<> struct transaction_identifier<kad2_hello_res> { static const uint16_t id = 104; };

    // kademlia request/response
    template<> struct transaction_identifier<kademlia2_req> { static const uint16_t id = 107; };
    template<> struct transaction_identifier<kademlia2_res> { static const uint16_t id = 107; };


    // firewalled 
    template<> struct transaction_identifier<kad_firewalled_req> { static const uint16_t id = 'f';  };
    template<> struct transaction_identifier<kad_firewalled_res> { static const uint16_t id = 'f';  };
}

#endif //__KAD_PACKET_STRUCT__
