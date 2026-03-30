#ifndef CONTENTUPDATER_H
#define CONTENTUPDATER_H

#include <QObject>

#include "controller/contentcontroller.h"
#include "network/networkreceiver.h"

class ContentUpdater : public NetWorkReceiver
{
    Q_OBJECT
public:
    explicit ContentUpdater(ContentController* ctrl, QObject* parent= nullptr);

    NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg);
private:
    QPointer<ContentController> m_ctrl;
};

#endif // CONTENTUPDATER_H
