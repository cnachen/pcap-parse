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
    ObsObserver obs_observer;
    int i = 0;
    for (auto file : obs_observer.list_files_by_hostname("LC6-EDY0038")) {
        printf("%s\n", file.c_str());
        auto ret = obs_observer.get_file_url(file);
        printf("%s\n", ret.c_str());
        // obs_observer.download_file(ret, "/tmp/D" + std::to_string(i++) + ".tar.gz");
    }

    // auto locked_queue = new LockedQueue<std::string>();

    // std::vector<PcapParser *> pcap_parsers;
    // for (int i = 0; i < 4; i++) {
    //     auto packet_handler = new PacketHandler;
    //     packet_handler->add_handler(new SomeipProtocolHandler);

    //     auto pcap_parser = new PcapParser(packet_handler, locked_queue);
    //     pcap_parser->start();

    //     pcap_parsers.push_back(pcap_parser);
    // }

    // for (int i = 0; i < 4; i++) {
    //     locked_queue->push(std::string(argv[1]));
    //     std::this_thread::sleep_for(10ms);
    // }

    while (true) {}
}
