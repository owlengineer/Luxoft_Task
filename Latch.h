#ifndef LATCH_H
#define LATCH_H

#include <condition_variable>
#include <mutex>

#include "Logging.h"

class Latch {
public:
    void trigger() {
        std::lock_guard<std::mutex> guard(m);
        flag = true;
        cv.notify_one();
    }

    void wait() {
        std::unique_lock<std::mutex> lock(m);
        cv.wait(lock, [this]{ return flag; });
    }

private:
    std::condition_variable cv;
    std::mutex m;
    bool flag;
};

#endif //LATCH_H
