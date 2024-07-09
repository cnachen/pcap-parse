#include "object.h"
#include "someip.h"

void PacketHandler::handle(const byte *packet, uint32_t len, time_t timestamp) {
    // Remove Ethernet/IP/UDP headers
    int shift = 0x2e;
    packet += shift;
    len -= shift;

    bool processed = false;
    for (auto protocol_handler : protocol_handlers) {
        protocol_handler->fill(packet, len, timestamp);
        if (protocol_handler->is_this_protocol()) {
            protocol_handler->handle();
            processed = true;
            break;
        }
    }

    if (!processed) {
        // Handle error here
    }
}

void PacketHandler::add_handler(ProtocolHandler *protocol_handler) {
    protocol_handlers.insert(protocol_handler);
}
