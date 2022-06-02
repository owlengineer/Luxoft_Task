#ifndef MOVEBASE_H
#define MOVEBASE_H

#include "Logging.h"

class MoveBase {
public:
    MoveBase() {
        LOG(DEBUG) << "default constructor"  << std::endl;
    }

    ~MoveBase() {
        LOG(DEBUG) << "default destructor"  << std::endl;
    }

    MoveBase(const MoveBase &other) {
        LOG(DEBUG) << "copy constructor"  << std::endl;
    }

    MoveBase(MoveBase &&other) {
        LOG(DEBUG) << "move constructor"  << std::endl;
    }

    MoveBase& operator=(const MoveBase &other) {
        LOG(DEBUG) << "copy assignment"  << std::endl;
        return *this;
    }

    MoveBase& operator=(MoveBase &&other) {
        LOG(DEBUG) << "move assignment"  << std::endl;
        return *this;
    }
};

#endif //MOVEBASE_H
