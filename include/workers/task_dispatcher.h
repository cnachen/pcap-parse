#ifndef _WORKERS_TASK_DISPATCHER_H
#define _WORKERS_TASK_DISPATCHER_H

#include <functional>
#include <vector>
#include <chrono>
#include <ctime>

#include "utility.h"
#include "workers/worker.h"

struct Task {
    using func_type = std::function<void(void *)>;
    using time_point_type = std::chrono::time_point<std::chrono::system_clock>;
    using interval_type = std::chrono::seconds;
    std::string name;
    func_type func;
    void *arg;
    time_point_type trigger_time_point;
    std::chrono::seconds trigger_interval;
    bool repetitive;
    bool valid = true;

    template<typename T>
    Task(std::string name,
         func_type func,
         void *arg,
         time_point_type trigger_time_point,
         T trigger_interval,
         bool repetitive = true) :
            name(name),
            func(func),
            arg(arg),
            trigger_time_point(trigger_time_point),
            trigger_interval(std::chrono::duration_cast<std::chrono::seconds>(trigger_interval)),
            repetitive(repetitive) {}
};

class TaskDispatcher : public Worker {
private:
    std::vector<Task> tasks;
protected:
    void work() override;
public:
    void add_task(Task task) {
        tasks.push_back(task);
    }
};

#endif