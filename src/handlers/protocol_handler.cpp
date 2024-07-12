#include <netinet/ip.h>

#include "object.h"
#include "someip.h"
#include "lib.h"

void ProtocolHandler::fill(const byte *packet, uint32_t len, double timestamp, std::string hostname) {
    this->packet = packet;
    this->len = len;
    this->timestamp = timestamp;
    this->hostname = hostname;
}
