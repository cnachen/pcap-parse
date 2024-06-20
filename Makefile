all:
	mkdir -p build && \
	cmake -D CMAKE_BUILD_TYPE=Debug -B build && \
	cmake --build build && \
	cp build/pcap_parse .

clean:
	cmake --build build --target clean