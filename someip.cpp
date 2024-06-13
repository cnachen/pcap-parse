#include <pcap.h>
#include <jsoncpp/json/json.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <unistd.h>
#include <string.h>
#include <printf.h>
#include <array>
#include <vector>
#include <unordered_set>

using byte = unsigned char;

std::array<char, 64> hostname;
Json::Value root;

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
    struct SomeipHeader *header;
    const byte *payload;
    struct SomeipSdPayload sdpayload;
};

// Legal SOME/IP service ids
const std::unordered_set<uint16_t> someip_service_ids {
    0xc101, 0xc102, 0xc103, 0xc104,
    0xc105, 0xc106, 0xc107, 0xc108,
    0xc109, 0xc10a, 0x8104, 0x8105,
    0x8108, 0x8109, 0x8101, 0x8102,
    0xffff,
};

// Legal SOME/IP-SD service entry ids
const std::unordered_set<uint8_t> someipsd_service_entry_ids {
    0x00, 0x01,
};

// Legal SOME/IP-SD eventgroup ids
const std::unordered_set<uint8_t> someipsd_eventgroup_entry_ids {
    0x06, 0x07,
};

bool parse_if_is_someip_packet(struct SomeipContext *ctx) {
    // SOME/IP header is at least 16 bytes
    if (ctx->len < 16)
        return false;

    struct SomeipHeader *header = ctx->header;
    memcpy(header, ctx->packet, sizeof(struct SomeipHeader));

    // To host byte order 
    header->service_id = ntohs(header->service_id);
    header->method_id = ntohs(header->method_id);
    header->length = ntohl(header->length);
    header->client_id = ntohs(header->client_id);
    header->session_id = ntohs(header->session_id);
    
    // Service id is illegal
    if (someip_service_ids.find(header->service_id) == someip_service_ids.end())
        return false;

    if (header->protocol_version != 0x1)
        return false;
    
    // Is SOME/IP-SD
    if (header->service_id == 0xffff) {
        ctx->protocol_type = ProtocolType::SOMEIPSD;
        ctx->sdpayload.flag = ctx->payload[0];
        ctx->sdpayload.entries_array_len = ntohl(*(uint32_t *)(ctx->payload + 4));
        int entry_length = sizeof(struct SomeipSdEntry);
        for (int i = 0; i < ctx->sdpayload.entries_array_len / entry_length; i++) {
            struct SomeipSdEntry entry;
            memcpy(&entry, ctx->payload + 8 + i * entry_length, entry_length);
            entry.service_id = ntohs(entry.service_id);
            entry.instance_id = ntohs(entry.instance_id);

            if (someipsd_service_entry_ids.find(entry.type) != someipsd_service_entry_ids.end()) {
                entry.service.minor_version = ntohl(entry.service.minor_version);
            } else if (someipsd_eventgroup_entry_ids.find(entry.type) != someipsd_eventgroup_entry_ids.end()) {
                entry.eventgroup.eventgroup_id = ntohs(entry.eventgroup.eventgroup_id);
            }

            ctx->sdpayload.entries_array.push_back(entry);
        }
    }

    return true;
}

void format_json_string(const Json::Value &json) {
    Json::StreamWriterBuilder builder;
    builder["commentStyle"] = "None";
    builder["indentation"] = "   ";
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    std::ostringstream os;
    writer->write(json, &os);
    std::string json_string = os.str();
    printf("%s", json_string.c_str());
}

Json::Value create_someip_json(struct SomeipContext *ctx) {
    Json::Value json;
    struct SomeipHeader *header = ctx->header;
    json["hostname"] = hostname.data();
    json["timestamp"] = ctx->timestamp;
    json["protocol_type"] = int(ctx->protocol_type);
    json["service_id"] = header->service_id;
    json["method_id"] = header->method_id;
    json["length"] = header->length;
    json["client_id"] = header->client_id;
    json["session_id"] = header->session_id;
    json["protocol_version"] = header->protocol_version;
    json["interface_version"] = header->interface_version;
    json["message_type"] = header->message_type;
    json["return_code"] = header->return_code;

    if (ctx->protocol_type == ProtocolType::SOMEIP) {
        json["payload"] = "someip";
    } else if (ctx->protocol_type == ProtocolType::SOMEIPSD) {
        Json::Value json_sdpayload;
        json_sdpayload["flag"] = ctx->sdpayload.flag;
        json_sdpayload["entries_array_len"] = ctx->sdpayload.entries_array_len;
        Json::Value json_entries;

        for (auto &entry : ctx->sdpayload.entries_array) {
            Json::Value json_entry;
            json_entry["type"] = entry.type;
            json_entry["service_id"] = entry.service_id;
            json_entry["instance_id"] = entry.instance_id;
            json_entry["major_version"] = entry.major_version;

            if (someipsd_service_entry_ids.find(entry.type) != someipsd_service_entry_ids.end()) {
                json_entry["minor_version"] = entry.service.minor_version;
            } else if (someipsd_eventgroup_entry_ids.find(entry.type) != someipsd_eventgroup_entry_ids.end()) {
                json_entry["counter"] = entry.eventgroup.counter;
                json_entry["eventgroup_id"] = entry.eventgroup.eventgroup_id;
            }

            json_entries.append(json_entry);
        }

        json_sdpayload["entries"] = json_entries;
        json["payload"] = json_sdpayload;
    }

    root.append(json);
    return json;
}

void handle_packet(const byte *packet, uint32_t len, time_t timestamp) {
    struct SomeipHeader header;

    // Remove Ethernet/IP/UDP headers
    int shift = 0x2e;
    packet += shift;
    len -= shift;

    struct SomeipContext ctx = {
        .protocol_type = ProtocolType::SOMEIP,
        .packet = packet,
        .len = len,
        .timestamp = timestamp,
        .header = &header,
        .payload = packet + 16,   
    };

    if (!parse_if_is_someip_packet(&ctx)) {
        // printf("Bad packet\n");
        return;
    }

    Json::Value json = create_someip_json(&ctx);
}

int main(int argc, char *argv[]) {
    // Get hostname for one time
    gethostname(hostname.data(), hostname.size());

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pcap file>\n", argv[0]);
        return -1;
    }

    char errbuf[PCAP_ERRBUF_SIZE];
    const char *file_name = argv[1];
    pcap_t *handle;

    handle = pcap_open_offline(file_name, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Couldn't open pcap file '%s': %s\n", file_name, errbuf);
        return -2;
    }

    const byte *packet;
    struct pcap_pkthdr header;

    // Handle each packet
    while ((packet = pcap_next(handle, &header)) != NULL) {
        handle_packet(packet, header.len, header.ts.tv_sec);
    }

    pcap_close(handle);

    // DEBUG
    format_json_string(root);
    
    return 0;
}
