all:
	mkdir -p build && \
	cmake -D CMAKE_BUILD_TYPE=Debug -B build && \
	cmake --build build

clean:
	cmake --build build --target clean

install:
	install -Dm755 build/pcap_parse /bin