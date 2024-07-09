#ifndef _WORKERS_PCAP_PARSER_H
#define _WORKERS_PCAP_PARSER_H

#include <thread>

#include "workers/worker.h"
#include "handlers/packet_handler.h"
#include "utility.h"

using namespace std::literals::chrono_literals;

class PcapParser : public Worker {
private:
    PacketHandler *packet_handler;
    LockedQueue<std::string> *locked_queue;
protected:
    void work() override {
        while (running) {
            auto [not_default, filename] = locked_queue->pop_default();
            if (not_default) {
                parse(filename);
            }
            std::this_thread::sleep_for(1s);
        }
    }
public:
    PcapParser(PacketHandler *packet_handler, LockedQueue<std::string> *locked_queue)
        : packet_handler(packet_handler), locked_queue(locked_queue) {}
    int parse(const std::string &filename);
};
#endif
