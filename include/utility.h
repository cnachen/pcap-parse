#ifndef _UTILITY_H
#define _UTILITY_H

#include <mutex>
#include <thread>
#include <queue>
#include <utility>
#include <functional>

#include "workers/worker.h"

template<typename T>
class LockedQueue {
public:
    bool empty() {
        return locked_scope([this] {
            return this->q.empty();
        });
    }
    int size() {
        return locked_scope([this] {
            return this->q.size();
        });
    }
    void push(T t) {
        return locked_scope([this, t] {
            this->q.push(t);
        });
    }
    T pop() {
        return locked_scope([this] {
            auto ret = this->q.front();
            this->q.pop();
            return ret;
        });
    }
    std::pair<bool, T> pop_default() {
        return locked_scope([this] {
            if (this->q.empty())
                return std::pair<bool, T>(false, T());
            auto ret = this->q.front();
            this->q.pop();
            return std::pair<bool, T>(true, ret);
        }); 
    }
private:
    std::queue<T> q;
    std::mutex m;
    template<typename Func, typename... Args>
    auto locked_scope(Func f, Args&&... args) -> decltype(f()) {
        std::lock_guard<std::mutex> lock(m);
        return f(std::forward<Args>(args)...);
    }
};

#endif