#include <pcap.h>
#include <jsoncpp/json/json.h>

#include <netinet/udp.h>
#include <unistd.h>
#include <string.h>
#include <printf.h>
#include <array>
#include <vector>
#include <unordered_set>

#include "object.h"
#include "someip.h"
#include "lib.h"

int PcapParser::parse(const std::string &filename) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;

    handle = pcap_open_offline(filename.c_str(), errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Couldn't open pcap file '%s': %s\n", filename.c_str(), errbuf);
        return -2;
    }

    const byte *packet;
    struct pcap_pkthdr header;
    
    // Handle each packet
    while ((packet = pcap_next(handle, &header)) != NULL) {
        this->packet_handler->handle(packet, header.len, header.ts.tv_sec);
    }

    // Send remaining packets before cleanup
    pcap_close(handle);

    return 0;
}
