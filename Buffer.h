#ifndef BUFFER_H
#define BUFFER_H

#include "Logging.h"

#include <string>
#include <vector>

class Buffer {
public:
    Buffer()
    {
        buffer.reserve(1000);
    }

    void serializeByte(uint8_t v) {
        buffer.push_back(v);
    }

    void serializeWord(uint16_t v) {
        buffer.push_back(v >> 8);
        buffer.push_back(v & 0xFF);
    }

    void serializeBytes(uint8_t *buf, size_t len) {
        for (int i = 0; i < len; ++i) {
            buffer.push_back(buf[i]);
        }
    }

    void deserializeBytes(uint8_t *buf, size_t len) {
        for (int i = 0; i < len; ++i) {
            uint8_t b;
            deserializeByte(b);
            buf[i] = b;
        }
    }

    void deserializeByte(uint8_t &v) {
        v = buffer.at(pos++);
    }

    void deserializeWord(uint16_t &v) {
        v = (buffer.at(pos) << 8) + buffer.at(pos + 1);
        pos += 2;
    }

    void serializeString(std::string v) {
        buffer.push_back(v.length());
        for (int i = 0; i < v.length(); ++i) {
            buffer.push_back(v[i]);
        }
    }

    void deserializeString(std::string &v) {
        uint8_t l;
        deserializeByte(l);
        v.resize(l);
        for (int i = 0; i < l; ++i) {
            uint8_t c;
            deserializeByte(c);
            v[i] = c;
        }
    }

    size_t getSize() {
        return buffer.size();
    }

    uint8_t* getBuffer() {
        return &buffer[0];
    }

    void dump() {
        std::cout << "Dump pos:" << pos << ", size:" << buffer.size() << std::endl;
        int c = 0;
        for (int i = 0; i < buffer.size(); ++i) {
            std::cout << static_cast<int>(buffer[i]) << " ";
            if ((i + 1) % 8 == 0) {
                std::cout << " ";
            }
            if (((i + 1) % 16 == 0) || i == buffer.size() - 1) {
                std::cout << std::endl;
            }
        }
    }

private:
    std::vector<uint8_t> buffer;
    size_t pos;
};

#endif //BUFFER_H
