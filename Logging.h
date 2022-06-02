#ifndef LOGGING_H
#define LOGGING_H

#include "string"
#include "stack"

#include "iostream"

#define LOG(level) get_logger(level)

enum {
    EXCEPTION = 0,
    ERROR,
    INFO,
    DEBUG
};

constexpr int logLevel = ERROR;

std::ostream& get_logger(int level);

class LoggingContext {
public:
    LoggingContext(const std::string &s) {
        context.push(s);
        LOG(INFO) << "ENTER: " << context.top() << std::endl << std::flush;
    }

    ~LoggingContext() {
        last = context.top();
        LOG(INFO) << "LEAVE: " << context.top() << std::endl << std::flush;
        context.pop();
    }

//private:
    thread_local static std::stack<std::string> context;
    thread_local static std::string last;
};

#define CONTEXT_BEGIN(CONTEXT) \
try { \
LoggingContext lc(CONTEXT);

#define CONTEXT_END() \
} catch (const std::exception &e) { \
LOG(EXCEPTION) << "Exception handled " << e.what() << " in " <<  LoggingContext::last << std::endl; \
}

#define ASSERT_EQUAL(a, b) \
{ \
if (a != b) { \
    LOG(ERROR) << "ERROR in " << __FILE__ << __LINE__ << " received '0x" << std::hex << (int)a << " expected '0x" << std::hex << (int)b << "'" << std::endl << std::flush; \
} \
}

#endif //LOGGING_H
