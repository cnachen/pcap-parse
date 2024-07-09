all:
	mkdir -p build && \
	cmake -G Ninja -D CMAKE_BUILD_TYPE=Debug -B build && \
	cmake --build build

clean:
	cmake --build build --target clean
	rm -rf build

install:
	install -Dm755 build/bip-eck /bin

docker:
	sudo docker build -f Dockerfile . -t bip_eck
