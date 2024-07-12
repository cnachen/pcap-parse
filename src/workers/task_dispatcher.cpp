#include <cstdio>

#include "workers/task_dispatcher.h"

#include <iomanip>
#include <iostream>

using namespace std::literals::chrono_literals;

void TaskDispatcher::work() {
    while (running) {
        std::chrono::time_point now = std::chrono::system_clock::now();
        
        for (auto &task : tasks) {
            // printf("%s %d\n", task.name.c_str(), task.valid);
            if (task.valid && task.trigger_time_point <= now) {
                task.func(task.arg);
                if (task.repetitive) {
                    task.trigger_time_point += task.trigger_interval;
                } else {
                    task.valid = false;
                }
            }
        }

        std::this_thread::sleep_for(1s);
    }
}
