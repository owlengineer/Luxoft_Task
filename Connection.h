#ifndef CONNECTION_H
#define CONNECTION_H

#include <queue>
#include <mutex>
#include <condition_variable>

class Connection {
public:
    Connection(const char *address, uint16_t port)
#ifdef USE_ADDR
    : m_Address(address)
#endif
    {};

    ~Connection() = default;

    void sendBuffer(const uint8_t* buf, size_t len) {
        std::lock_guard<std::mutex> lg(mut);
        for (int i = 0; i < len; ++i) {
            queue.push(buf[i]);
        }
        cv.notify_all();
    }

    void receiveBuffer(uint8_t* buf, size_t len) {
        std::unique_lock<std::mutex> lg(mut);
        int l = 0;
        do {
            cv.wait(lg, [this]{ return !queue.empty(); });
            while (!queue.empty() && (l < len)) {
                buf[l] = queue.front();
                queue.pop();
                ++l;
            }
        } while (l < len);
    }

private:
    std::queue<uint8_t> queue;
    std::mutex mut;
    std::condition_variable cv;

#ifdef USE_ADDR
public:
    std::string m_Address;
#endif
};

#endif //CONNECTION_H
