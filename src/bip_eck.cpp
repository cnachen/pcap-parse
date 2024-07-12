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
    auto local_files = new LockedQueue<LocalFile>;

    ObsObserver obs_observer(local_files);
    obs_observer.start();

    MongodbUploader mongodb_uploader;
    mongodb_uploader.start();

    std::vector<PcapParser *> pcap_parsers;
    for (int i = 0; i < 4; i++) {
        auto packet_handler = new PacketHandler;
        packet_handler->add_handler(new SomeipProtocolHandler);

        auto pcap_parser = new PcapParser(packet_handler, local_files);
        pcap_parser->start();

        pcap_parsers.push_back(pcap_parser);
    }

    // TaskDispatcher td;
    // td.start();

    // Task task("test", [](void *arg) {
    //     printf("1 now = %ld\n", std::time(NULL));
    // }, &td, std::chrono::system_clock::now(), 1s);

    // td.add_task(task);

    std::this_thread::sleep_for(3650 * 24h);
}
