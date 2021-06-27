#include "connectionaccepter.h"

ConnectionAccepter::ConnectionAccepter() : m_next(nullptr) {}
ConnectionAccepter::~ConnectionAccepter()
{
    if(nullptr != m_next)
    {
        delete m_next;
        m_next= nullptr;
    }
}
void ConnectionAccepter::setNext(ConnectionAccepter* next)
{
    m_next= next;
}

bool ConnectionAccepter::isActive() const
{
    return m_isActive;
}

void ConnectionAccepter::setIsActive(bool isActive)
{
    m_isActive= isActive;
}
bool ConnectionAccepter::runAccepter(const QMap<QString, QVariant>& data) const
{
    bool result= isValid(data);
    if(nullptr != m_next)
        result&= m_next->runAccepter(data);
    return result;
}
