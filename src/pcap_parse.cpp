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

#include "lib.h"
#include "someip.h"

Global g;

bool parse_if_is_someip_packet(SomeipContext *ctx) {
    // SOME/IP header is at least 16 bytes
    if (ctx->len < sizeof(SomeipHeader))
        return false;

    struct SomeipHeader *header = &ctx->header;
    memcpy(header, ctx->packet, sizeof(SomeipHeader));

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
        // 8 bytes, so we'll start from here after
        ctx->protocol_type = ProtocolType::SOMEIPSD;
        ctx->sdpayload.flag = ctx->payload[0];
        ctx->sdpayload.entries_array_len = ntohl(*(uint32_t *)(ctx->payload + 4));

        int entry_length = sizeof(SomeipSdEntry);
        for (int i = 0; i < ctx->sdpayload.entries_array_len / entry_length; i++) {
            SomeipSdEntry entry;
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

Json::Value create_someip_json(SomeipContext *ctx) {
    Json::Value json;
    SomeipHeader *header = &ctx->header;
    json["hostname"] = g.hostname.data();
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
        json["payload"] = tohex(ctx->payload, ctx->len - sizeof(SomeipHeader));
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

    return json;
}

void handle_packet(const byte *packet, uint32_t len, time_t timestamp) {
    // Remove Ethernet/IP/UDP headers
    int shift = 0x2e;
    packet += shift;
    len -= shift;

    SomeipContext ctx = {
        .protocol_type = ProtocolType::SOMEIP,
        .packet = packet,
        .len = len,
        .timestamp = timestamp,
        .payload = packet + sizeof(SomeipHeader),   
    };

    if (!parse_if_is_someip_packet(&ctx)) {
        // printf("Bad packet\n");
        return;
    }

    Json::Value json = create_someip_json(&ctx);
    g.root.append(json);
}

int main(int argc, char *argv[]) {
    get_obs_file("1234.jpg");

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
        post_mongodb(g, g.root);
    }

    // Send remaining packets before cleanup
    post_mongodb(g, g.root);
    pcap_close(handle);

    // DEBUG
    // printf("%s\n", format_json_string(g.root).c_str());
    
    return 0;
}
