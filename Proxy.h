#ifndef SERVICEPROXY_H
#define SERVICEPROXY_H

#include "ServiceCommon.h"

#include "Skeleton.h"

#include "AbstractProxy.h"

#include "Logging.h"
#include "Buffer.h"
#include "Connection.h"

#include <map>

class Proxy : public AbstractProxy {
public:
    Proxy(std::shared_ptr<Connection>& connCS, std::shared_ptr<Connection>& connSC)
    : AbstractProxy(connCS, connSC)
    {}

    std::shared_future<Method_Out>
    method1(const Method1_In &in) {
        LOG(INFO) << "Calling method1()" << std::endl;

        uint16_t id = UniqueId::getUniqueId();
        std::shared_ptr<Buffer> buffer{std::make_shared<Buffer>()};
        std::shared_ptr<Request> s{std::make_shared<Request>(id, buffer)};

        buffer->serializeWord(leadWord);
        buffer->serializeWord(id);
        buffer->serializeWord(3);
        buffer->serializeWord(serviceA::MethodIds::METHOD1);
        buffer->serializeByte(in.a);
        buffer->serializeWord(trailWord);

        //activeRequests.(id, s);
        return sendRequest(s);
    }

    std::shared_future<Method_Out>
    method2(const Method2_In &in) {
        LOG(INFO) << "Calling method1()" << std::endl;

        uint16_t id = UniqueId::getUniqueId();
        std::shared_ptr<Buffer> buffer{std::make_shared<Buffer>()};
        std::shared_ptr<Request> s{std::make_shared<Request>(id, buffer)};

        buffer->serializeWord(leadWord);
        buffer->serializeWord(id);
        buffer->serializeWord(4);
        buffer->serializeWord(serviceA::MethodIds::METHOD2);
        buffer->serializeByte(in.x);
        buffer->serializeByte(in.y);
        buffer->serializeWord(trailWord);

        return sendRequest(s);
    }

    typedef std::function<void (const Method_Out &out)> Method1;
    std::shared_future<Method_Out>
    method1(const Method1_In &in, Method1 callback) {
        m_callback = callback;
        return method1(in);
    }

    virtual void onMethod1ResultAvailable(const Method_Out &out) {}
    virtual void onMethod2ResultAvailable(const Method_Out &out) {}

    void onResultAvailable(const Method_Out &out) {
        if (m_callback) {
            m_callback(out);
        }
        LOG(INFO) << "Calling onMethod1ResultAvailable()" << std::endl;
    }

    virtual void doBufferProcessing(uint16_t id, Buffer buffer) override {
        // Do buffer decode and call for methods here

        uint16_t t;
        buffer.deserializeWord(t);
        serviceA::MethodIds m;
        m = static_cast<serviceA::MethodIds>(t);

        std::shared_ptr<Request> sourceRequest = activeRequests[id];

        switch (m) {
            case serviceA::MethodIds::METHOD1:
            {
                uint8_t r;
                buffer.deserializeByte(r);
                const Method_Out &out{r};
                // sourceRequest->getPromiseResult()->set_value(out);
                onResultAvailable(out);
                onMethod1ResultAvailable(out);
                break;
            }
            case serviceA::MethodIds::METHOD2: {
                uint8_t r;
                buffer.deserializeByte(r);
                const Method_Out &out{r};
                // sourceRequest->getPromiseResult()->set_value(out);
                onResultAvailable(out);
                onMethod2ResultAvailable(out);
                break;
            }

            default:
                LOG(ERROR) << "Unknown method id in response: " << m << std::endl;
        }

    }

private:
    Method1 m_callback{};
    // static std::atomic<int> totalActive1;
    // std::shared_ptr<Connection> connectionClientServer;

    std::map<uint16_t, std::shared_ptr<Request>> activeRequests;
};

#endif //SERVICEPROXY_H
