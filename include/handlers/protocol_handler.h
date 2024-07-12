#ifndef _WORKERS_PROTOCOL_HANDLER_H
#define _WORKERS_PROTOCOL_HANDLER_H

#include <unordered_set>

#include "handlers/protocol_handler.h"
#include "lib.h"

class ProtocolHandler {
private:
    const byte *packet = nullptr;
    uint32_t len = 0;
    double timestamp = 0;
protected:
    std::string hostname;
public:
    virtual bool is_this_protocol() {
        return false;
    }
    virtual void handle() {}
    virtual void fill(const byte *packet, uint32_t len, double timestamp, std::string hostname);
};

#endif
