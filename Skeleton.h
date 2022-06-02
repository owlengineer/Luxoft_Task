#ifndef SKELETON_H
#define SKELETON_H

#include "ServiceCommon.h"
#include "AbstractSkeleton.h"

#include "Buffer.h"

class Skeleton : public AbstractSkeleton {
public:
    Skeleton(std::shared_ptr<Connection>& connCS, std::shared_ptr<Connection>& connSC)
    : AbstractSkeleton(connCS, connSC) {}

    virtual Method_Out method1(const Method1_In &in) = 0;
    virtual Method_Out method2(const Method2_In &in) = 0;

    virtual void doBufferProcessing(uint16_t id, Buffer buffer) override {
        // Do buffer decode and call for methods here

        uint16_t t;
        buffer.deserializeWord(t);
        serviceA::MethodIds m;
        m = static_cast<serviceA::MethodIds>(t);

        switch (m) {
            case serviceA::MethodIds::METHOD1: {
                uint8_t b;
                buffer.deserializeByte(b);
                const Method1_In &in{b};
                Method_Out out = method1(in);
                sendResponseMethod1(id, out);
                break;
            }

            case serviceA::MethodIds::METHOD2: {
                uint8_t x;
                buffer.deserializeByte(x);
                uint8_t y;
                buffer.deserializeByte(y);
                const Method2_In &in{x, y};
                Method_Out out = method2(in);
                sendResponseMethod2(id, out);
                break;
            }

            default:
                LOG(ERROR) << "Unknown method id: " << m << std::endl;
        }
    }

    std::shared_future<Method_Out>
    sendResponseMethod1(uint16_t id, const Method_Out &out) {
        LOG(INFO) << "Calling response for method1()" << std::endl;

        std::shared_ptr<Buffer> buffer{std::make_shared<Buffer>()};
        std::shared_ptr<Request> s{std::make_shared<Request>(id, buffer)};

        buffer->serializeWord(leadWord);
        buffer->serializeWord(id);
        buffer->serializeWord(3);
        buffer->serializeWord(serviceA::MethodIds::METHOD1);
        buffer->serializeByte(out.r);
        buffer->serializeWord(trailWord);

        return sendRequest(s);
    }

    std::shared_future<Method_Out>
    sendResponseMethod2(uint16_t id, const Method_Out &out) {
        LOG(INFO) << "Calling response for method1()" << std::endl;

        std::shared_ptr<Buffer> buffer{std::make_shared<Buffer>()};
        std::shared_ptr<Request> s{std::make_shared<Request>(id, buffer)};

        buffer->serializeWord(leadWord);
        buffer->serializeWord(id);
        buffer->serializeWord(3);
        buffer->serializeWord(serviceA::MethodIds::METHOD2);
        buffer->serializeByte(out.r);
        buffer->serializeWord(trailWord);

        return sendRequest(s);
    }

};

#endif //SKELETON_H
