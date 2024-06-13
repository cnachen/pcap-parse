all:
	c++ pcap_parse.cpp -lpcap -ljsoncpp -o pcap_parse -std=gnu++20
