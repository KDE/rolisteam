/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "controller/networkcontroller.h"

#include <QLoggingCategory>
#include <QMetaObject>
#include <QThread>
#include <chrono>
#include <iostream>
#include <thread>

#include "controller/gamecontroller.h"
#include "controller/playercontroller.h"
#include "network/clientmanager.h"
#include "network/networkmessage.h"
#include "network/receiveevent.h"
#include "network/rserver.h"
#include "services/ipchecker.h"
#include "utils/countdownobject.h"
#include "worker/iohelper.h"
#include "worker/messagehelper.h"
#include "worker/modelhelper.h"
#include "worker/networkhelper.h"
#include "worker/playermessagehelper.h"

QLoggingCategory rNetwork("rolisteam.network");

void readDataAndSetModel(NetworkMessageReader* msg, ChannelModel* model)
{
    auto data= msg->byteArray32();
    auto jon= IOHelper::textByteArrayToJsonObj(data);
    helper::network::fetchChannelModel(model, jon);
}

NetworkController::NetworkController(QObject* parent)
    : AbstractControllerInterface(parent)
    , m_clientManager(new ClientManager)
    //, m_hbSender(new HeartBeatSender)
    , m_profileModel(new ProfileModel)
    , m_channelModel(new ChannelModel)
    , m_countDown(new CountDownObject(5, 10))
{
    qRegisterMetaType<RServer::ServerState>();
    SettingsHelper::readConnectionProfileModel(m_profileModel.get());

    ReceiveEvent::registerNetworkReceiver(NetMsg::AdministrationCategory, this);

    connect(m_clientManager.get(), &ClientManager::connectionStateChanged, this,
            [this](ClientManager::ConnectionState state)
            {
                qDebug() << "NETWORKCONTROLLER - ConnectionState" << state;
                setConnected(state == ClientManager::AUTHENTIFIED);
                setConnecting(state == ClientManager::CONNECTING);
            });

    connect(m_clientManager.get(), &ClientManager::dataReceived, this, &NetworkController::downloadingData);
    connect(m_clientManager.get(), &ClientManager::messageReceived, this, &NetworkController::dispatchMessage);
    connect(m_clientManager.get(), &ClientManager::connectedToServer, this, &NetworkController::sendOffConnectionInfo);
    connect(m_clientManager.get(), &ClientManager::gameMasterStatusChanged, this, &NetworkController::isGMChanged);
    connect(m_clientManager.get(), &ClientManager::moveToAnotherChannel, this, &NetworkController::tableChanged);
    connect(m_countDown.get(), &CountDownObject::triggered, this, &NetworkController::startServer);

    connect(this, &NetworkController::selectedProfileIndexChanged, this, [this]() { emit isGMChanged(isGM()); });
    connect(this, &NetworkController::selectedProfileIndexChanged, this, &NetworkController::hostingChanged);
    connect(this, &NetworkController::selectedProfileIndexChanged, this, &NetworkController::askForGMChanged);
    connect(this, &NetworkController::selectedProfileIndexChanged, this, &NetworkController::hostChanged);
    connect(this, &NetworkController::selectedProfileIndexChanged, this, &NetworkController::portChanged);
    connect(this, &NetworkController::selectedProfileIndexChanged, this, &NetworkController::serverPasswordChanged);

    connect(m_clientManager.get(), &ClientManager::authentificationSuccessed, this,
            [this]() { setGroups(m_currentGroups | Group::ADMIN); });
}
NetworkController::~NetworkController() {}

void NetworkController::dispatchMessage(QByteArray array)
{
    NetworkMessageReader data;
    data.setData(array);
    if(ReceiveEvent::hasNetworkReceiverFor(data.category()))
    {
        QList<NetWorkReceiver*> tmpList= ReceiveEvent::getNetWorkReceiverFor(data.category());
        for(NetWorkReceiver* tmp : std::as_const(tmpList))
        {
            tmp->processMessage(&data);
        }
    }
}

ProfileModel* NetworkController::profileModel() const
{
    return m_profileModel.get();
}

ChannelModel* NetworkController::channelModel() const
{
    return m_channelModel.get();
}

void NetworkController::startConnection()
{
    if(hosting() && currentProfile())
    {
        currentProfile()->setAddress(QStringLiteral("localhost"));
        m_countDown->start();
    }
    else
        startClient();
}

void NetworkController::setConnected(bool b)
{
    if(b == m_connected)
        return;
    m_connected= b;
    emit connectedChanged(m_connected);
}

void NetworkController::setConnecting(bool b)
{
    if(b == m_connecting)
        return;
    m_connecting= b;
    emit connectingChanged(m_connecting);
}

void NetworkController::setAdminPassword(const QByteArray& array)
{
    if(m_admindPw == array)
        return;
    m_admindPw= array;
    emit adminPasswordChanged();
}

void NetworkController::removeProfile(int pos)
{
    m_profileModel->removeProfile(pos);
}

void NetworkController::startClient()
{
    m_clientManager->connectTo(host(), port());
}

void NetworkController::stopClient()
{
    m_clientManager->disconnectAndClose();
}

void NetworkController::startServer()
{
    m_serverParameters= {{"ServerPassword", serverPassword()}, {"AdminPassword", adminPassword()}};

    if(!m_server)
    {
        m_server.reset(new RServer(m_serverParameters, true));
        m_serverThread.reset(new QThread);
    }
    m_server->moveToThread(thread());
    m_server->setPort(port());

    connect(m_serverThread.get(), &QThread::started, m_server.get(), &RServer::listen);
    connect(m_serverThread.get(), &QThread::finished, this,
            [this]() { emit infoMessage("server thread has been closed"); });

    connect(m_server.get(), &RServer::finished, this, [this]() { emit infoMessage("server has been closed"); });

    connect(m_server.get(), &RServer::stateChanged, this,
            [this]()
            {
                switch(m_server->state())
                {
                case RServer::Stopped:
                    m_serverThread->quit();
                    break;
                case RServer::Listening:
                    m_countDown->stop();
                    emit infoMessage("server is on");
                    startClient();
                    break;
                case RServer::Error:
                    closeServer();
                    break;
                }
            });

    m_ipChecker.reset(new IpChecker());
    connect(m_ipChecker.get(), &IpChecker::ipAddressChanged, this,
            [this](const QString& ip)
            {
                m_ipv4Address= ip;
                emit ipv4Changed();
            });
    m_ipChecker->startCheck();

    m_server->moveToThread(m_serverThread.get());
    m_serverThread->start();
}

void NetworkController::stopConnecting()
{
    if(!m_connecting)
        return;

    stopConnection();
}

void NetworkController::stopConnection()
{
    if(!m_connected && !m_connecting)
        return;

    stopClient();
    if(hosting())
    {
        closeServer();
    }
}

NetWorkReceiver::SendType NetworkController::processMessage(NetworkMessageReader* msg)
{
    NetWorkReceiver::SendType type= NetWorkReceiver::NONE;
    switch(msg->action())
    {
    case NetMsg::EndConnectionAction:
        break;
    case NetMsg::SetChannelList:
        readDataAndSetModel(msg, m_channelModel.get());
        break;
    case NetMsg::AdminAuthSucessed:
        qDebug() << "Authentification fail";
        setGroups(m_currentGroups | ADMIN);
        break;
    case NetMsg::AuthentificationFail:
        qDebug() << "Authentification fail";
        m_clientManager->setAuthentificationStatus(false);
        break;
    case NetMsg::AuthentificationSucessed:
        qDebug() << "Authentification sucessed";
        m_clientManager->setAuthentificationStatus(true);
        break;
    case NetMsg::HeartbeatAsk:
    {
        NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::HeartbeatAnswer);
        msg.sendToServer();
    }
    break;
    default:
        break;
    }

    return type;
}

bool NetworkController::hosting() const
{
    return currentProfile() ? currentProfile()->isServer() : false;
}

bool NetworkController::askForGM() const
{
    return currentProfile() ? currentProfile()->isGM() : false;
}

QString NetworkController::host() const
{
    return currentProfile() ? currentProfile()->address() : QString();
}

int NetworkController::port() const
{
    return currentProfile() ? currentProfile()->port() : 6660;
}

QString NetworkController::ipv4() const
{
    return m_ipv4Address;
}

QString NetworkController::lastError() const
{
    return m_lastError;
}

bool NetworkController::isGM() const
{
    return currentProfile() ? currentProfile()->isGM() : false;
}

bool NetworkController::connected() const
{
    return m_connected;
}

bool NetworkController::connecting() const
{
    return m_connecting;
}

QByteArray NetworkController::serverPassword() const
{
    return currentProfile() ? currentProfile()->password() : QByteArray();
}

QByteArray NetworkController::adminPassword() const
{
    return m_admindPw;
}

void NetworkController::setGameController(GameController* game)
{
    m_gameCtrl= game;
}

void NetworkController::sendOffConnectionInfo()
{
    if(m_gameCtrl.isNull())
        return;

    auto playerCtrl= m_gameCtrl->playerController();
    if(nullptr == playerCtrl)
        return;
    PlayerMessageHelper::sendOffConnectionInfo(playerCtrl->localPlayer(), serverPassword());
}

void NetworkController::setLastError(const QString& error)
{
    if(error == m_lastError)
        return;
    m_lastError= error;
    emit lastErrorChanged(m_lastError);
}
void NetworkController::closeServer()
{
    if(!m_server)
        return;

    QMetaObject::invokeMethod(m_server.get(), &RServer::close, Qt::QueuedConnection);
}

void NetworkController::saveData()
{
    SettingsHelper::writeConnectionProfileModel(m_profileModel.get());
}

void NetworkController::sendOffLoginAdmin(const QString& password)
{
    auto pwA= QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha3_512);

    NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::AdminPassword);
    msg.byteArray32(pwA);
    msg.sendToServer();
}

void NetworkController::lockChannel(const QString& uuid, NetMsg::Action action)
{
    NetworkMessageWriter msg(NetMsg::AdministrationCategory, action);
    msg.string8(uuid);
    msg.sendToServer();
}

void NetworkController::banUser(const QString& uuid, const QString& playerId)
{
    NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::BanUser);
    msg.string8(uuid);
    msg.string8(playerId);
    msg.sendToServer();
}

void NetworkController::kickUser(const QString& uuid, const QString& playerId)
{
    NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::Kicked);
    msg.string8(uuid);
    msg.string8(playerId);
    msg.sendToServer();
}

void NetworkController::addChannel(const QString& parentId) {}

void NetworkController::resetChannel(const QString& channelId)
{
    NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::ResetChannel);
    msg.string8(channelId);
    msg.sendToServer();
}

void NetworkController::deleteChannel(const QString& channelId)
{
    NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::DeleteChannel);
    msg.string8(channelId);
    msg.sendToServer();
}

void NetworkController::definePasswordOnChannel(const QString& channelId, const QByteArray& password)
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

int NetworkController::selectedProfileIndex() const
{
    return m_selectedProfileIndex;
}

void NetworkController::setSelectedProfileIndex(int newSelectedProfileIndex)
{
    if(m_selectedProfileIndex == newSelectedProfileIndex)
        return;
    m_selectedProfileIndex= newSelectedProfileIndex;
    emit selectedProfileIndexChanged();
}

ConnectionProfile* NetworkController::currentProfile() const
{
    return m_profileModel->getProfile(m_selectedProfileIndex);
}

bool NetworkController::isAdmin() const
{
    return (m_currentGroups & ADMIN);
}

NetworkController::Groups NetworkController::groups() const
{
    return m_currentGroups;
}
void NetworkController::setGroups(NetworkController::Groups group)
{
    if(m_currentGroups == group)
        return;
    m_currentGroups= group;
    emit groupsChanged();
}
