#ifndef _WORKERS_PACKET_HANDLER_H
#define _WORKERS_PACKET_HANDLER_H

#include <unordered_set>

#include "handlers/protocol_handler.h"

class PacketHandler {
private:
    std::unordered_set<ProtocolHandler *> protocol_handlers;
public:
    void handle(const byte *packet, uint32_t len, time_t timestamp);
    void add_handler(ProtocolHandler *protocol_handler);
};

#endif
