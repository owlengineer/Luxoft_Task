#include <queue>
#include <future>
#include <iostream>

#include "Latch.h"

#include "MoveBase.h"
#include "Proxy.h"

#include "Server.h"

#include "Connection.h"

#include <chrono>
using namespace std::chrono_literals;

#define VARIANT_FUTURE_GET 1
#define VARIANT_CLIENT_METHOD 2
#define VARIANT_CLIENT_METHOD_TWO_SERVICES 3
#define VARIANT_LAMBDA 4
#define VARIANT_LAMBDA_CHAIN 5

#define VARIANT VARIANT_CLIENT_METHOD_TWO_SERVICES

Latch completion;

std::shared_ptr<Connection> connectionClientServer{std::make_shared<Connection>("1:1:1:1", 1010)};
std::shared_ptr<Connection> connectionServerClient{std::make_shared<Connection>("2:2:2:2", 2020)};

#if VARIANT == VARIANT_CLIENT_METHOD
class Client : public Proxy {
public:
    Client(std::shared_ptr<Connection>& connCS, std::shared_ptr<Connection>& connSC) : Proxy(connCS, connSC) {}

    virtual void onMethod1ResultAvailable(const Method_Out &out) override {
        std::cout << "Result: " << out.r << std::endl << std::flush;
        completion.trigger();
    }
 };
#endif

#if VARIANT == VARIANT_CLIENT_METHOD_TWO_SERVICES
class Client : public Proxy {
public:
    Client(std::shared_ptr<Connection>& connCS, std::shared_ptr<Connection>& connSC) : Proxy(connCS, connSC) {}

    virtual void onMethod1ResultAvailable(const Method_Out &out) override {
        std::cout << "Result1: " << out.r << std::endl << std::flush;
        completion.trigger();
    }

    virtual void onMethod2ResultAvailable(const Method_Out &out) override {
        std::cout << "Result2: " << out.r << std::endl << std::flush;
        completion.trigger();
    }
};
#endif

int main() {
    CONTEXT_BEGIN("main")

    std::cout << "main-2: " << std::endl << std::flush;

    Server server(connectionClientServer, connectionServerClient);
    Client client1(connectionClientServer, connectionServerClient);
    
    // several methods
    std::shared_future<Method_Out> resFuture1(client1.method1({5}));
    std::shared_future<Method_Out> resFuture2(client1.method2({10, 10}));
    std::shared_future<Method_Out> resFuture3(client1.method2({200, 10}));

    std::this_thread::sleep_for(2s);

    completion.trigger();
    completion.wait();

    CONTEXT_END()

    return 0;
}
