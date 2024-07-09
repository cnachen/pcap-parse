#include <netinet/ip.h>

#include "object.h"
#include "someip.h"
#include "lib.h"

extern Global g;

void ProtocolHandler::fill(const byte *packet, uint32_t len, time_t timestamp) {
    this->packet = packet;
    this->len = len;
    this->timestamp = timestamp;
}
