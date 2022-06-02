#ifndef MIDDLEWARE1_ABSTRACTSKELETON_H
#define MIDDLEWARE1_ABSTRACTSKELETON_H

#include "Sender.h"
#include "Receiver.h"

class AbstractSkeleton : public Receiver
#ifdef EXTENDED
        , public Sender
#endif
        {
public:
    AbstractSkeleton(std::shared_ptr<Connection>& connCS, std::shared_ptr<Connection>& connSC)
    : Receiver(connCS)
#ifdef EXTENDED
    , Sender(connSC)
#endif
    {}
};

#endif //MIDDLEWARE1_ABSTRACTSKELETON_H
