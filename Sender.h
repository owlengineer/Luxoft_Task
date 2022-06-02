#ifndef MESSAGESENDER_H
#define MESSAGESENDER_H

#include "ServiceCommon.h"

#include "Skeleton.h"

#include "Logging.h"
#include "Buffer.h"
#include "Connection.h"

#include <set>

class Sender {
public:
    Sender(std::shared_ptr<Connection>& conn)
      : finish{false}
      , connection(conn)
    {
        processorThread = std::thread([&]{
            sending();
        });
    }

    ~Sender() {
        finish = true;
        cv.notify_all();
        processorThread.join();
    }

    void sending() {
        while (true) {
            CONTEXT_BEGIN("Sending Loop")

                std::shared_ptr<Request> request;
                {
                    std::unique_lock<std::mutex> lk(m);
                    cv.wait(lk,
                            [this] { return !requests.empty() || finish; }
                    );

                    if (finish && requests.empty()) {
                        //LOG(INFO) << "Exiting processor thread" << std::endl;
                        break;
                    }

                    request = requests.front();
                    requests.pop_front();
                    inProgress.insert(request);
                    cv.notify_all(); // Not sure that is really need
                }

            connection->sendBuffer(request->getBuffer().getBuffer(), request->getBuffer().getSize());

            cv.notify_all();
            CONTEXT_END()
        }
    }

    std::shared_future<Method_Out>
    sendRequest(std::shared_ptr<Request> request) {
        LOG(INFO) << "Calling sendRequest()" << std::endl;
        std::lock_guard<std::mutex> lk(m);

        LOG(INFO) << "Push new queue element..." << std::endl;
        requests.push_back(request);
        cv.notify_all();

        return requests.back()->getFutureResult();
    }

    void dump() {
        LOG(INFO) << "Done requests dump:" << std::endl;
        for (auto &request: requests) {
            LOG(INFO) << "  " << request->getRequestId() << std::endl;
        }

        LOG(INFO) << "... end" << std::endl;
    }

private:
    std::thread processorThread;
    std::mutex m;
    std::deque<std::shared_ptr<Request>> requests;
    std::set<std::shared_ptr<Request>> inProgress;
    std::condition_variable cv;

    std::atomic<bool> finish;

//protected:
    std::shared_ptr<Connection> connection;
};

#endif //MESSAGESENDER_H
