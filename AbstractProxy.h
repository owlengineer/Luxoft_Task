#ifndef MIDDLEWARE1_ABSTRACTPROXY_H
#define MIDDLEWARE1_ABSTRACTPROXY_H

#include "Sender.h"
#include "Receiver.h"

class AbstractProxy
        : public Sender
#ifdef EXTENDED
        , public Receiver
#endif
        {
public:
    AbstractProxy(std::shared_ptr<Connection>& connCS, std::shared_ptr<Connection>& connSC)
    : Sender(connCS)
#ifdef EXTENDED
    , Receiver(connSC)
#endif
    {
    }
};

#endif //MIDDLEWARE1_ABSTRACTPROXY_H
