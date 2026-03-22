#ifndef NETWORKUPDATER_H
#define NETWORKUPDATER_H

#include <QObject>

#include "controller/networkcontroller.h"
#include "network/networkmessage.h"

class NetworkUpdater : public QObject
{
    Q_OBJECT
public:
    explicit NetworkUpdater(NetworkController* ctrl, QObject* parent= nullptr);

signals:

public slots:
    void sendOffConnectionInfo(const QString& id, const QString& name, const QByteArray& password);
    void sendOffLoginAdmin(const QString& password);
    void lockChannel(const QString& uuid, NetMsg::Action action);
    void banUser(const QString& uuid, const QString& playerId);
    void kickUser(const QString& uuid, const QString& playerId);
    void addChannel(const QString& parentId= QString());
    void resetChannel(const QString& channelId);
    void deleteChannel(const QString& channelId);
    void definePasswordOnChannel(const QString& channelId, const QByteArray& password);
    void defineChannelAsDefault(const QString& channelId);
    void joinChannel(const QString& userId, const QString& channelId, const QByteArray& password);
    // void userLeftChannel(const QString& channelId, const QString& userId);

private:
    QPointer<NetworkController> m_ctrl;
};

#endif // NETWORKUPDATER_H
