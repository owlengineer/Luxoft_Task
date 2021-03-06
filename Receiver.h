#ifndef MESSAGERECEIVER_H
#define MESSAGERECEIVER_H

#include "ServiceCommon.h"

#include "Skeleton.h"

#include "Logging.h"
#include "Buffer.h"
#include "Connection.h"

#define THREADS_NUM 2

class Receiver {
public:
    Receiver(std::shared_ptr<Connection>& conn)
            : finish{false}
            , connection(conn)
    {
        readingThread = std::thread([&]{
            receiving();
        });

	// A num of processing threads
	for (uint32_t i = 0; i < THREADS_NUM; ++i) 
        	taskThreads.emplace_back(&Receiver::processing, this);

    }

    ~Receiver() {
        finish = true;
        cv.notify_all();
	for (uint32_t i = 0; i < THREADS_NUM; ++i) 
        	taskThreads[i].join();
	readingThread.join();
    }

    void doTask1() {
    	
    }

    void receiving() {
        while (true) {
            CONTEXT_BEGIN("Receiving Loop")
            std::shared_ptr<Request> request;
            {
                uint8_t guardBuf[sizeof(uint16_t)];
                connection->receiveBuffer(guardBuf, sizeof(uint16_t));
                ASSERT_EQUAL((guardBuf[0] << 8) + guardBuf[1], leadWord)

                uint8_t szBuf[sizeof(uint16_t)];
                connection->receiveBuffer(szBuf, sizeof(uint16_t));
                uint16_t id = (szBuf[0] >> 8) + szBuf[1];
                LOG(INFO) << "RECEIVED ID" << id << "!" << std::endl << std::flush;

                connection->receiveBuffer(szBuf, sizeof(uint16_t));
                uint16_t sz = (szBuf[0] >> 8) + szBuf[1];
                LOG(INFO) << "RECEIVED SIZE" << sz << "!" << std::endl << std::flush;

                uint8_t buf[1024];
                connection->receiveBuffer(buf, sz);
                LOG(INFO) << "RECEIVED BUFFER" << std::endl;

                connection->receiveBuffer(guardBuf, sizeof(uint16_t));
                ASSERT_EQUAL((guardBuf[0] << 8) + guardBuf[1], trailWord)

                if (finish && requests.empty()) {
                    LOG(INFO) << "Exiting reading thread" << std::endl;
                    break;
                }

                std::shared_ptr<Buffer> buffer(std::make_shared<Buffer>());
                buffer->serializeBytes(buf, sz);
                //Requester("method1", buffer);
                
		std::shared_ptr<Request> request(std::make_shared<Request>(id, buffer));
                
		std::unique_lock<std::mutex> lk(m);
                requests.push_back(request);
		lk.unlock();

                cv.notify_one(); // Not sure that is really need
            }
            CONTEXT_END()
        }
    }

    void processing() {
        //LOG(ERROR) << "PROCESSING STARTED" << std::endl;
        while (true) {
            CONTEXT_BEGIN("Processing Loop"
#ifdef USE_ADDR
             + connection->m_Address
#endif
            )
            std::shared_ptr<Request> request;
            {
                std::unique_lock<std::mutex> lk(m);
                cv.wait(lk,
                        [this] { return !requests.empty() || finish; }
                );

                if (finish && requests.empty()) {
                    LOG(ERROR) << "Exiting processor thread" << std::endl;
                    break;
                }

                request = requests.front();
                requests.pop_front();
		lk.unlock();
		cv.notify_all(); // Not sure that is really need

                LOG(INFO) << "BUFFER PROCESSING" << std::endl << std::flush;
              	doBufferProcessing(request->getRequestId(), request->getBuffer());
                //TODO Buffer to argument
            }

//            const Method1_In& in = request->getMethod1_In();
//            //Method1_Out out{25}; //serviceSkeleton.method1(in);
//            skeleton->method1(in);
//            request->getPromiseResult().set_value(out);

            LOG(INFO) << "Calling onResultAvailable()" << std::endl;

            cv.notify_all();
            CONTEXT_END()
        }
    }

    virtual void doBufferProcessing(uint16_t id, Buffer buffer) {}

    void dump() {
        LOG(INFO) << "Done requests dump:" << std::endl;
        for (auto &request: requests) {
            LOG(INFO) << "  " << request->getRequestId() << std::endl;
        }

        LOG(INFO) << "... end" << std::endl;
    }

private:
    std::thread readingThread;
    std::mutex m;
    std::deque<std::shared_ptr<Request>> requests;
    //std::deque<std::shared_ptr<Request>> doneRequests;
    std::condition_variable cv;

    std::atomic<bool> finish;
    
    //std::queue<std::unique_ptr<std::thread>> taskThreads;
    std::vector<std::thread> taskThreads;

//protected:
    std::shared_ptr<Connection> connection;
};

#endif //MESSAGERECEIVER_H
