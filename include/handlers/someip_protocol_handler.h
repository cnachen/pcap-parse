#ifndef _WORKERS_SOMEIP_PROTOCOL_HANDLER_H
#define _WORKERS_SOMEIP_PROTOCOL_HANDLER_H

#include <unordered_set>

#include "handlers/protocol_handler.h"
#include "someip.h"

class SomeipProtocolHandler : public ProtocolHandler {
private:
    SomeipContext context;
    SomeipHeader *header;
    Json::Value json;
    int chunk_size = 256;
public:
    bool is_this_protocol() override;
    void handle() override;
    void fill(const byte *packet, uint32_t len, double timestamp, std::string hostname) override;
    void to_json();
};

#endif
