#ifndef HEARTBEATSENDER_H
#define HEARTBEATSENDER_H

#include <QObject>
#include <QTimer>

#include "preferences/preferenceslistener.h"
#include "preferences/preferencesmanager.h"
/**
 * @brief The heartBeatSender class is dedicated to send off heartbeat messages preventing
 * disconnection for some users.
 */
class HeartBeatSender : public QObject, public PreferencesListener
{
    Q_OBJECT
public:
    explicit HeartBeatSender(QObject* parent= nullptr);

    void preferencesHasChanged(const QString&) override;
    void updateStatus();

    void setIdLocalUser(QString);

public slots:

    void sendHeartBeatMsg();
    void updateTimer();

private:
    QTimer m_timer;
    PreferencesManager* m_preferences= nullptr;
    int m_timeOut;
    QString m_localId= QStringLiteral("unknown");
    bool m_status;
};

#endif // HEARTBEATSENDER_H
