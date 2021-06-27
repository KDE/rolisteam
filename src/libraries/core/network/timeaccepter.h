#ifndef TIMEACCEPTER_H
#define TIMEACCEPTER_H

#include "connectionaccepter.h"

class TimeAccepter : public ConnectionAccepter
{
public:
    TimeAccepter();

    virtual bool isValid(const QMap<QString, QVariant>& data) const override;
};

#endif // TIMEACCEPTER_H
