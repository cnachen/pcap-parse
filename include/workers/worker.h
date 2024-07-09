#ifndef _WORKERS_WORKER_H
#define _WORKERS_WORKER_H

#include <thread>

class Worker {
public:
    Worker() : running(false) {}
    virtual ~Worker();
    void start();
    void stop();
protected:
    volatile bool running;
    virtual void work();
private:
    std::thread worker_thread;
};

#endif
