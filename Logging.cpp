#include "Logging.h"

class NullStream : public std::ostream {
private:
    class NullBuffer : public std::streambuf {
    public:
        int overflow(int c) override { return c; }
    } buffer_;

public:
    NullStream() : std::ostream(&buffer_) {}
} nullStream;


std::ostream& get_logger(int level) {
    return logLevel >= level ? std::cout : nullStream;
}

thread_local std::stack<std::string> LoggingContext::context;
thread_local std::string LoggingContext::last;
