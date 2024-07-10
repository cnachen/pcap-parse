#include <stdio.h>
#include <chrono>
#include <vector>
#include <thread>

#include "object.h"
#include "cloud.h"
#include "utility.h"

using namespace std::literals::chrono_literals;

Global g;

int main(int argc, char *argv[]) {
    auto filenames = new LockedQueue<std::string>;

    ObsObserver obs_observer(filenames);
    obs_observer.start();

    MongodbUploader mongodb_uploader;
    mongodb_uploader.start();

    std::vector<PcapParser *> pcap_parsers;
    for (int i = 0; i < 4; i++) {
        auto packet_handler = new PacketHandler;
        packet_handler->add_handler(new SomeipProtocolHandler);

        auto pcap_parser = new PcapParser(packet_handler, filenames);
        pcap_parser->start();

        pcap_parsers.push_back(pcap_parser);
    }

    std::this_thread::sleep_for(3650 * 24h);
}
