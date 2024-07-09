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
public:
    bool is_this_protocol() override;
    void handle() override;
    void fill(const byte *packet, uint32_t len, time_t timestamp) override;
    void to_json();
};

#endif
