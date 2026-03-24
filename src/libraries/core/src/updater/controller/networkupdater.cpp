#include "updater/controller/networkupdater.h"

#include <QCryptographicHash>
#include <QThread>

#include "worker/modelhelper.h"
#include "worker/playermessagehelper.h"

NetworkUpdater::NetworkUpdater(NetworkController* ctrl, QObject* parent) : QObject{parent}, m_ctrl(ctrl)
{
    SettingsHelper::readConnectionProfileModel(m_ctrl->profileModel());
    connect(m_ctrl, &NetworkController::addChannel, this, &NetworkUpdater::addChannel);
    connect(m_ctrl, &NetworkController::lockChannel, this, &NetworkUpdater::lockChannel);
    connect(m_ctrl, &NetworkController::sendOffLoginAdmin, this, &NetworkUpdater::sendOffLoginAdmin);
    connect(m_ctrl, &NetworkController::banUser, this, &NetworkUpdater::banUser);
    connect(m_ctrl, &NetworkController::kickUser, this, &NetworkUpdater::kickUser);
    connect(m_ctrl, &NetworkController::resetChannel, this, &NetworkUpdater::resetChannel);
    connect(m_ctrl, &NetworkController::deleteChannel, this, &NetworkUpdater::deleteChannel);
    connect(m_ctrl, &NetworkController::definePasswordOnChannel, this, &NetworkUpdater::definePasswordOnChannel);
    connect(m_ctrl, &NetworkController::sendOffConnectionInfo, this, &NetworkUpdater::sendOffConnectionInfo);
    connect(m_ctrl, &NetworkController::joinChannel, this, &NetworkUpdater::joinChannel);
    connect(m_ctrl, &NetworkController::saveData, this,
            [this]() { SettingsHelper::writeConnectionProfileModel(m_ctrl->profileModel()); });
    connect(m_ctrl, &NetworkController::defineChannelAsDefault, this, &NetworkUpdater::defineChannelAsDefault);

    auto channels= ctrl->channelModel();
    connect(channels, &ChannelModel::channelNameChanged, this,
            [](const QString& id, const QString& name)
            {
                NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::RenameChannel);
                msg.string8(id);
                msg.string32(name);
                msg.sendToServer();
            });
}

void NetworkUpdater::sendOffConnectionInfo(const QString& id, const QString& name, const QByteArray& password)
{
    PlayerMessageHelper::sendOffConnectionInfo(id, name, password);
}

void NetworkUpdater::sendOffLoginAdmin(const QString& password)
{
    auto pwA
        = password.isEmpty() ? QByteArray() : QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha3_512);

    NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::AdminPassword);
    msg.byteArray32(pwA);
    msg.sendToServer();
}

void NetworkUpdater::lockChannel(const QString& uuid, NetMsg::Action action)
{
    NetworkMessageWriter msg(NetMsg::AdministrationCategory, action);
    msg.string8(uuid);
    msg.sendToServer();
}

void NetworkUpdater::banUser(const QString& uuid, const QString& playerId)
{
    NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::BanUser);
    msg.string8(uuid);
    msg.string8(playerId);
    msg.sendToServer();
}

void NetworkUpdater::kickUser(const QString& uuid, const QString& playerId)
{
    NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::KickUser);
    msg.string8(uuid);
    msg.string8(playerId);
    msg.sendToServer();
}

void NetworkUpdater::addChannel(const QString& parentId)
{
    NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::AddChannel);
    msg.string8(parentId);
    PlayerMessageHelper::writeChannelInMsg(msg, QUuid::createUuid().toString(), tr("New channel"), QString());
    msg.sendToServer();
}

void NetworkUpdater::resetChannel(const QString& channelId)
{
    NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::ResetChannel);
    msg.string8(channelId);
    msg.sendToServer();
}

void NetworkUpdater::deleteChannel(const QString& channelId)
{
    NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::DeleteChannel);
    msg.string8(channelId);
    msg.sendToServer();
}

void NetworkUpdater::definePasswordOnChannel(const QString& channelId, const QByteArray& password)
{
    if(password.isEmpty())
    {
        NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::ResetChannelPassword);
        msg.string8(channelId);
        msg.sendToServer();
        return;
    }

    NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::ChannelPassword);
    msg.string8(channelId);

    msg.byteArray32(password);
    msg.sendToServer();
}

void NetworkUpdater::joinChannel(const QString& userId, const QString& channelId, const QByteArray& password)
{
    NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::JoinChannel);
    msg.string8(channelId);
    msg.string8(userId);
    msg.byteArray32(password);
    msg.sendToServer();
}

void NetworkUpdater::defineChannelAsDefault(const QString& channelId)
{
    NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::SetDefaultChannel);
    msg.string8(channelId);
    msg.sendToServer();
}

/*void NetworkUpdater::userLeftChannel(const QString& channelId, const QString& userId)
{
    Q_UNUSED(channelId)
    NetworkMessageWriter msg(NetMsg::UserCategory, NetMsg::DelPlayerAction);
    msg.string8(userId);
    msg.sendToServer();
}*/
