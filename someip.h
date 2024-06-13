#ifndef _SOMEIP_H
#define _SOMEIP_H

#include <cstdint>
#include <vector>
#include <unordered_set>

#include "lib.h"

enum class ProtocolType {
    SOMEIP,
    SOMEIPSD,
};

// Set alignment to 1
#pragma pack(push, 1)
struct SomeipHeader {
    uint16_t service_id;
    uint16_t method_id;
    uint32_t length;
    uint16_t client_id;
    uint16_t session_id;
    uint8_t protocol_version;
    uint8_t interface_version;
    uint8_t message_type;
    uint8_t return_code;
};

struct SomeipSdEntry {
    uint8_t type;
    uint8_t index1;
    uint8_t index2;
    uint8_t opts;
    uint16_t service_id;
    uint16_t instance_id;
    uint8_t major_version;
    uint8_t ttl[3];
    union {
        struct {
            uint32_t minor_version;
        } service;
        struct {
            uint16_t reserved: 12;
            uint16_t counter: 4;
            uint16_t eventgroup_id;
        } eventgroup;  
    };
};

struct SomeipSdOption {
    uint16_t length;
    uint8_t type;
    uint8_t reserved1;
    uint32_t ipv4_address;
    uint8_t reserved2;
    uint16_t l4_protocol;
    uint16_t port_number;
};

struct SomeipSdPayload {
    uint8_t flag;
    uint8_t reversed[3];
    uint32_t entries_array_len;
    std::vector<SomeipSdEntry> entries_array;
    // uint32_t options_array_len;
    // std::vector<SomeipSdOption> options_array;
};
#pragma pack(pop)

struct SomeipContext {
    // 0: SOME/IP
    // 1: SOME/IP-SD
    ProtocolType protocol_type;
    const byte *packet;
    uint32_t len;
    time_t timestamp;
    SomeipHeader header;
    const byte *payload;
    SomeipSdPayload sdpayload;
};

// Legal SOME/IP service ids
static const std::unordered_set<uint16_t> someip_service_ids {
    0xc101, 0xc102, 0xc103, 0xc104,
    0xc105, 0xc106, 0xc107, 0xc108,
    0xc109, 0xc10a, 0x8104, 0x8105,
    0x8108, 0x8109, 0x8101, 0x8102,
    0xffff,
};

// Legal SOME/IP-SD service entry ids
static const std::unordered_set<uint8_t> someipsd_service_entry_ids {
    0x00, 0x01,
};

// Legal SOME/IP-SD eventgroup ids
static const std::unordered_set<uint8_t> someipsd_eventgroup_entry_ids {
    0x06, 0x07,
};

#endif /* _SOMEIP_H */