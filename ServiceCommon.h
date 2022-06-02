#ifndef SERVICECOMMON_H
#define SERVICECOMMON_H

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <future>
#include <iostream>
#include <mutex>
#include <thread>

#include "MoveBase.h"

#include "Logging.h"
#include "Buffer.h"

using namespace std::chrono_literals;

#define EXTENDED
#define USE_ADDR

constexpr uint16_t leadWord = 0x1234;
constexpr uint16_t trailWord = 0x5678;

namespace serviceA {

enum MethodIds : uint16_t {
    METHOD1 = 1,
    METHOD2 = 2,
};

namespace method1 {
}

}

class Method1_In {
public:
    int a;
};

class Method2_In {
public:
    int x, y;
};

class Method_Out {
public:
    int r;
};

//void Requester(const std::string &id, std::shared_ptr<Buffer>& buf) {}

class Request : public MoveBase {
public:
    Request() = default;
    ~Request() = default;

    Request(const Request& r)
            : requestId(r.requestId), promiseResult(), futureResult(promiseResult->get_future())
            //, method1_In(r.method1_In), method1_Out(r.method1_Out)
    {};

    Request& operator=(const Request&) = delete;
    Request& operator=(Request&&) = default;

    Request(uint16_t reqId, std::shared_ptr<Buffer>& buf)
            : requestId(reqId)
            , buffer(buf)
//            , method1_In(in)
            , promiseResult{std::make_shared<std::promise<Method_Out>>()}
            , futureResult(promiseResult->get_future())
    {
        Method_Out out{47};
        promiseResult->set_value(out); // HACK
    }

    std::shared_future<Method_Out> getFutureResult() {
        return futureResult;
    }

    std::shared_ptr<std::promise<Method_Out>> getPromiseResult() {
        return promiseResult;
    }

    const uint16_t getRequestId() {
        return requestId;
    }

    Buffer& getBuffer() {
        return *buffer;
    }

private:
    uint16_t requestId;
    std::shared_ptr<std::promise<Method_Out>> promiseResult;
    std::shared_future<Method_Out> futureResult;
    std::shared_ptr<Buffer> buffer;
};

class UniqueId {
public:
    static uint16_t getUniqueId() {
        counter++;
        return counter;
    }

private:
    static std::atomic<uint16_t> counter;
};

#endif //SERVICECOMMON_H
