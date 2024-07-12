#include <netinet/ip.h>

#include "object.h"
#include "someip.h"
#include "lib.h"

extern Global g;

bool SomeipProtocolHandler::is_this_protocol() {
    // SOME/IP header is at least 16 bytes
    if (this->context.len < sizeof(SomeipHeader))
        return false;

    // Service id is illegal
    if (someip_service_ids.find(header->service_id) == someip_service_ids.end())
        return false;

    if (header->protocol_version != 0x1)
        return false;

    return true;
}

void SomeipProtocolHandler::handle() {
    to_json();
    g.jsons.push(json);
}

void SomeipProtocolHandler::fill(const byte *packet, uint32_t len, double timestamp, std::string hostname) {
    this->ProtocolHandler::fill(packet, len, timestamp, hostname);

    this->context = {
        .protocol_type = ProtocolType::SOMEIP,
        .packet = packet,
        .len = len,
        .timestamp = timestamp,
        .payload = packet + sizeof(SomeipHeader),   
    };

    header = &this->context.header;

    memcpy(header, this->context.packet, sizeof(SomeipHeader));

    // To host byte order 
    header->service_id = ntohs(header->service_id);
    header->method_id = ntohs(header->method_id);
    header->length = ntohl(header->length);
    header->client_id = ntohs(header->client_id);
    header->session_id = ntohs(header->session_id);

    // Fill SOME/IP-SD fields
    if (header->service_id == 0xffff) {
        // 8 bytes, so we'll start from here after
        this->context.protocol_type = ProtocolType::SOMEIPSD;
        this->context.sdpayload.flag = this->context.payload[0];
        this->context.sdpayload.entries_array_len = ntohl(*(uint32_t *)(this->context.payload + 4));

        int entry_length = sizeof(SomeipSdEntry);
        for (int i = 0; i < 
        this->context.sdpayload.entries_array_len / entry_length; i++) {
            SomeipSdEntry entry;
            memcpy(&entry, this->context.payload + 8 + i * entry_length, entry_length);
            entry.service_id = ntohs(entry.service_id);
            entry.instance_id = ntohs(entry.instance_id);

            if (someipsd_service_entry_ids.find(entry.type) != someipsd_service_entry_ids.end()) {
                entry.service.minor_version = ntohl(entry.service.minor_version);
            } else if (someipsd_eventgroup_entry_ids.find(entry.type) != someipsd_eventgroup_entry_ids.end()) {
                entry.eventgroup.eventgroup_id = ntohs(entry.eventgroup.eventgroup_id);
            }

            this->context.sdpayload.entries_array.push_back(entry);
        }
    }
}

void SomeipProtocolHandler::to_json() {
    json.clear();
    json["hostname"] = this->hostname;
    json["timestamp"] = this->context.timestamp;
    json["protocol_type"] = int(this->context.protocol_type);
    json["service_id"] = header->service_id;
    json["method_id"] = header->method_id;
    json["length"] = header->length;
    json["client_id"] = header->client_id;
    json["session_id"] = header->session_id;
    json["protocol_version"] = header->protocol_version;
    json["interface_version"] = header->interface_version;
    json["message_type"] = header->message_type;
    json["return_code"] = header->return_code;

    if (this->context.protocol_type == ProtocolType::SOMEIP) {
        json["payload"] = tohex(this->context.payload, this->context.len - sizeof(SomeipHeader));
    } else if (this->context.protocol_type == ProtocolType::SOMEIPSD) {
        Json::Value json_sdpayload;
        json_sdpayload["flag"] = this->context.sdpayload.flag;
        json_sdpayload["entries_array_len"] = this->context.sdpayload.entries_array_len;
        Json::Value json_entries;

        for (auto &entry : this->context.sdpayload.entries_array) {
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
}
