#ifndef _WORKERS_PCAP_PARSER_H
#define _WORKERS_PCAP_PARSER_H

#include <thread>

#include "workers/worker.h"
#include "handlers/packet_handler.h"
#include "utility.h"

class PcapParser : public Worker {
private:
    PacketHandler *packet_handler;
    LockedQueue<std::string> *locked_queue;
protected:
    void work() override;
public:
    PcapParser(PacketHandler *packet_handler, LockedQueue<std::string> *locked_queue)
        : packet_handler(packet_handler), locked_queue(locked_queue) {}
    int parse(const std::string &filename);
};
#endif
