all:
	mkdir -p build && \
	cmake -D CMAKE_BUILD_TYPE=Debug -B build && \
	cmake --build build

clean:
	cmake --build build --target clean