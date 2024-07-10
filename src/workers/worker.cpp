#include <chrono>
#include <iostream>

#include "workers/worker.h"

Worker::~Worker() {
    stop();
}

void Worker::start() {
    running = true;
    worker_thread = std::thread(&Worker::work, this);
}

void Worker::stop() {
    running = false;
    if (worker_thread.joinable()) {
        worker_thread.join();
    }
}

void Worker::work() {
    while (running) {
        std::cout << "Working..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
