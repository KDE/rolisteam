﻿#include "servermanager.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>

#include "passwordaccepter.h"

#include "core/updater/controller/servermanagerupdater.h"
#include "ipbanaccepter.h"
#include "iprangeaccepter.h"
#include "network/networkmessagewriter.h"
#include "timeaccepter.h"

ServerManager::ServerManager(QObject* parent)
    : QObject(parent), m_server(nullptr), m_model(new ChannelModel), m_serverUpdater(new ServerManagerUpdater(this))
{
    qRegisterMetaType<NetworkMessage*>("NetworkMessage*");

    connect(m_model.get(), &ChannelModel::totalSizeChanged, this, &ServerManager::memoryChannelChanged);

    m_msgDispatcher= new MessageDispatcher(this);
    connect(this, &ServerManager::messageMustBeDispatched, m_msgDispatcher, &MessageDispatcher::dispatchMessage,
            Qt::QueuedConnection);

    connect(m_msgDispatcher, &MessageDispatcher::messageForAdmin, this, &ServerManager::processMessageAdmin);
    m_model->addChannel("default", "");

    PasswordAccepter* tmp2= new PasswordAccepter();

    m_corEndProcess.reset(tmp2);
    tmp2->setNext(nullptr);

    m_corConnection.reset(new IpBanAccepter());

    IpRangeAccepter* tmp= new IpRangeAccepter();
    TimeAccepter* tmp3= new TimeAccepter();
    m_corConnection->setNext(tmp);
    tmp->setNext(tmp3);
    tmp3->setNext(nullptr);

    m_adminAccepter.reset(new PasswordAccepter(PasswordAccepter::Admin));
    m_adminAccepter->setNext(nullptr);
}

ServerManager::~ServerManager()
{
    stopListening();
}

int ServerManager::port() const
{
    return m_port;
}

int ServerManager::tryCount() const
{
    return m_tryCount;
}

void ServerManager::setPort(int p)
{
    if(p == m_port)
        return;
    m_port= p;
    emit portChanged();
}

void ServerManager::setTryCount(int tryCount)
{
    if(m_tryCount == tryCount)
        return;
    m_tryCount= tryCount;
    emit tryCountChanged();
}

void ServerManager::startListening()
{
    if(!m_server)
    {
        m_server.reset(new RServer(this, getValue(QStringLiteral("ThreadCount")).toInt()));
        connect(m_server.get(), &RServer::finished, this, [this]() { setState(Stopped); });
    }
    ++m_tryCount;
    if(m_server->listen(QHostAddress::Any, static_cast<quint16>(getValue(QStringLiteral("port")).toInt())))
    {
        setState(Listening);
        emit errorOccured(tr("Rolisteam Server is on!"), LogController::Info);
    }
    else
    {
        setState(Error);
        emit errorOccured(m_server->errorString(), LogController::Error);
        if(m_tryCount < getValue(QStringLiteral("TryCount")).toInt()
           || getValue(QStringLiteral("TryCount")).toInt() == 0)
        {
            emit errorOccured(tr("Retry start server in %1s!").arg(getValue(QStringLiteral("TimeToRetry")).toInt()),
                              LogController::Info);
            QTimer::singleShot(getValue(QStringLiteral("TimeToRetry")).toInt(), this, SLOT(startListening()));
        }
        else
        {
            emit errorOccured(tr("Retry count reached. Server stops trying."), LogController::Info);
            setState(Stopped); // on error
        }
    }
}
void ServerManager::stopListening()
{
    // m_server->refuseNewConnection(true);
    // close();
    if(m_server)
        m_server->terminate();
}

void ServerManager::messageReceived(QByteArray array)
{
    TcpClient* client= qobject_cast<TcpClient*>(sender());
    if(nullptr != client)
    {
        Channel* channel= client->getParentChannel();
        {
            emit messageMustBeDispatched(array, channel, client);
        }
    }
}

void ServerManager::initServerManager()
{
    // create channel
    int chCount= getValue("ChannelCount").toInt();
    int count= m_model->rowCount(QModelIndex());
    for(int i= count; i < chCount; ++i)
    {
        m_model->addChannel(QStringLiteral("Channel %1").arg(i), "");
    }
}

void ServerManager::initClient()
{
    TcpClient* client= qobject_cast<TcpClient*>(sender());
    if(nullptr != client)
    {
        qDebug() << "client insert" << client << client->getName();
        m_connections.insert(client->getSocket(), client);
        sendEventToClient(client, TcpClient::CheckSuccessEvent);
    }
    else
    {
        sendEventToClient(client, TcpClient::CheckFailEvent);
    }
}
void ServerManager::sendEventToClient(TcpClient* client, TcpClient::ConnectionEvent event)
{
    QMetaObject::invokeMethod(client, "sendEvent", Qt::QueuedConnection, Q_ARG(TcpClient::ConnectionEvent, event));
}

/////////////////////////////////////////////////////////
///
/// Slot to perform check during connection process.
///
////////////////////////////////////////////////////////
void ServerManager::serverAcceptClient(TcpClient* client)
{
    if(nullptr != client)
    {
        QMap<QString, QVariant> data(m_parameters);
        data["currentIp"]= client->getIpAddress();
        if(m_corConnection->runAccepter(data))
        {
            sendEventToClient(client, TcpClient::ControlSuccessEvent);
        }
        else
        {
            sendEventToClient(client, TcpClient::ControlFailEvent);
        }
    }
}
void ServerManager::checkAuthToServer(TcpClient* client)
{
    if(nullptr == client)
        return;

    QMap<QString, QVariant> data(m_parameters);
    data["currentIp"]= client->getIpAddress();
    data["userpassword"]= client->getServerPassword();
    if(m_corEndProcess->runAccepter(data))
    {
        m_model->addConnectionToDefaultChannel(client);
        sendEventToClient(client, TcpClient::ServerAuthSuccessEvent);
        // sendOffModel(client);
    }
    else
    {
        sendEventToClient(client, TcpClient::ServerAuthFailEvent);
    }
}
void ServerManager::checkAuthAsAdmin(TcpClient* client)
{
    QMap<QString, QVariant> data(m_parameters);
    data["userpassword"]= client->getAdminPassword();
    if(m_adminAccepter->runAccepter(data))
    {
        sendEventToClient(client, TcpClient::AdminAuthSuccessEvent);
    }
    else
    {
        sendEventToClient(client, TcpClient::AdminAuthFailEvent);
    }
}

void ServerManager::memoryChannelChanged(quint64 size)
{
    if(size > m_parameters["memorySize"].toULongLong())
    {
        m_model->emptyChannelMemory();
    }
}
void ServerManager::checkAuthToChannel(TcpClient* client, QString channelId, QByteArray password)
{
    QMap<QString, QVariant> data(m_parameters);
    auto item= m_model->getItemById(channelId);
    auto channel= dynamic_cast<Channel*>(item);

    auto eventToSend= TcpClient::ChannelAuthSuccessEvent;

    if(nullptr == channel)
    {
        eventToSend= TcpClient::ChannelAuthFailEvent;
        sendEventToClient(client, eventToSend);
        return;
    }

    if(channel->password() != password)
        eventToSend= TcpClient::ChannelAuthFailEvent;

    if((m_corEndProcess->runAccepter(data)) && (eventToSend != TcpClient::ChannelAuthFailEvent))
    {
        if(!m_model->addConnectionToChannel(channelId, client))
        {
            m_model->addConnectionToDefaultChannel(client);
        }
    }
    else
    {
        eventToSend= TcpClient::ChannelAuthFailEvent;
    }
    sendEventToClient(client, eventToSend);
}
/////////////////////////////////////////////////////////
///
/// Slot to perform check during connection process.
///
////////////////////////////////////////////////////////
void ServerManager::sendOffAdminAuthSuccessed()
{
    TcpClient* client= qobject_cast<TcpClient*>(sender());
    if(nullptr != client)
    {
        NetworkMessageWriter* msg= new NetworkMessageWriter(NetMsg::AdministrationCategory, NetMsg::AdminAuthSucessed);
        QMetaObject::invokeMethod(client, "sendMessage", Qt::QueuedConnection,
                                  Q_ARG(NetworkMessage*, static_cast<NetworkMessage*>(msg)), Q_ARG(bool, true));
        // sendOffModel(client);
    }
}
void ServerManager::sendOffAdminAuthFail()
{
    TcpClient* client= qobject_cast<TcpClient*>(sender());
    if(nullptr != client)
    {
        NetworkMessageWriter* msg= new NetworkMessageWriter(NetMsg::AdministrationCategory, NetMsg::AdminAuthFail);
        QMetaObject::invokeMethod(client, "sendMessage", Qt::QueuedConnection,
                                  Q_ARG(NetworkMessage*, static_cast<NetworkMessage*>(msg)), Q_ARG(bool, true));
    }
    emit errorOccured(
        tr("Authentification as Admin fails: %2 - %1, Wrong password.").arg(client->getName(), client->getIpAddress()),
        LogController::Info);
}
void ServerManager::sendOffAuthSuccessed()
{
    TcpClient* client= qobject_cast<TcpClient*>(sender());
    if(nullptr != client)
    {
        NetworkMessageWriter* msg
            = new NetworkMessageWriter(NetMsg::AdministrationCategory, NetMsg::AuthentificationSucessed);
        QMetaObject::invokeMethod(client, "sendMessage", Qt::QueuedConnection,
                                  Q_ARG(NetworkMessage*, static_cast<NetworkMessage*>(msg)), Q_ARG(bool, true));
        // sendOffModel(client);
    }
}
void ServerManager::sendOffAuthFail()
{
    TcpClient* client= qobject_cast<TcpClient*>(sender());
    if(nullptr != client)
    {
        NetworkMessageWriter* msg
            = new NetworkMessageWriter(NetMsg::AdministrationCategory, NetMsg::AuthentificationFail);
        QMetaObject::invokeMethod(client, "sendMessage", Qt::QueuedConnection,
                                  Q_ARG(NetworkMessage*, static_cast<NetworkMessage*>(msg)), Q_ARG(bool, true));
    }
    emit errorOccured(
        tr("Authentification fails: %1 try to connect to the server with wrong password.").arg(client->getIpAddress()),
        LogController::Info);
}
void ServerManager::kickClient(QString id, bool isAdmin, QString senderId)
{
    m_model->kick(id, isAdmin, senderId);
    // sendOffModelToAll();

    TcpClient* client= nullptr;
    auto keys= m_connections.keys();
    for(auto& key : keys)
    {
        auto value= m_connections[key];
        if(value->getId() == id)
        {
            client= value;
        }
    }
    emit errorOccured(tr("User has been kick out: %2 - %1.").arg(client->getName(), client->getIpAddress()),
                      LogController::Info);

    if(nullptr != client)
    {
        removeClient(client);
    }
}

void ServerManager::banClient(QString id, bool isAdmin, QString senderId)
{
    // TODO implement this function
    Q_UNUSED(id)
    Q_UNUSED(isAdmin)
    Q_UNUSED(senderId)
}

void ServerManager::processMessageAdmin(NetworkMessageReader* msg, Channel* chan, TcpClient* tcp)
{
    if(tcp == nullptr)
        return;

    bool isAdmin= tcp->isAdmin();
    bool isGM= tcp->isGM();
    auto sourceId= tcp->playerId();
    switch(msg->action())
    {
    case NetMsg::Kicked:
    {
        QString id= msg->string8();
        kickClient(id, isAdmin, sourceId);
    }
    break;
    case NetMsg::BanUser:
    {
        QString id= msg->string8();
        banClient(id, isAdmin, sourceId);
    }
    break;
    case NetMsg::RenameChannel:
    {
        if(isAdmin)
        {
            QString idChan= msg->string8();
            QString newName= msg->string32();
            m_model->renameChannel(sourceId, idChan, newName);
        }
    }
    break;
    case NetMsg::AddChannel:
    {
        if(isAdmin)
        {
            QString idparent= msg->string8();
            TreeItem* parentItem= m_model->getItemById(idparent);
            Channel* dest= static_cast<Channel*>(parentItem);

            auto channel= new Channel();
            // channel->read(*msg);
            m_model->addChannelToChannel(channel, dest);
        }
    }
    break;
    case NetMsg::JoinChannel:
    {
        QString id= msg->string8();
        QString idClient= msg->string8();
        TreeItem* item= m_model->getItemById(id);
        Channel* dest= static_cast<Channel*>(item);
        if(nullptr != dest && !dest->locked())
        {
            m_model->moveClient(chan, idClient, dest);
            sendEventToClient(tcp, TcpClient::ChannelChanged);
        }
    }
    break;
    case NetMsg::SetChannelList:
    {
        qDebug() << "Server received channellist";
        /*if(isAdmin)
        {
            QByteArray data= msg->byteArray32();
            QJsonDocument doc= QJsonDocument::fromJson(data);
            if(!doc.isEmpty())
            {
                QJsonObject obj= doc.object();
               // m_model->readDataJson(obj);
            }
        }*/
    }
    break;
    case NetMsg::DeleteChannel:
    {
        if(isAdmin)
        {
            QString id= msg->string8();
            m_model->removeChild(id);
            // sendOffModelToAll();
        }
    }
    break;
    case NetMsg::MoveChannel:
    case NetMsg::AdminPassword:
        break;
    case NetMsg::ResetChannel:
    {
        if(isGM)
        {
            if(nullptr != chan)
                chan->clearData();
        }
        else if(isAdmin)
        {
            QString id= msg->string8();
            auto item= m_model->getItemById(id);
            if(!item->isLeaf())
            {
                auto channel= dynamic_cast<Channel*>(item);
                if(channel)
                    channel->clearData();
            }
        }
    }
    break;
    case NetMsg::LockChannel:
    case NetMsg::UnlockChannel:
        if(isGM)
        {
            if(nullptr != chan)
                chan->setLocked(msg->action() == NetMsg::LockChannel ? true : false);
        }
        break;
    default:
        break;
    }
}

/*void ServerManager::sendOffModel(TcpClient* client)
{
    if(nullptr == client)
        return;

    qDebug() << "ServerManager Send off channel model" << sender();
    static QMap<TcpClient*, QByteArray> model;
    QJsonDocument doc;
    QJsonObject obj;
    m_model->writeDataJson(obj);
    doc.setObject(obj);

    auto b= doc.toJson();

    if(b != model[client])
    {
        model[client]= b;

        NetworkMessageWriter* msg= new NetworkMessageWriter(NetMsg::AdministrationCategory, NetMsg::SetChannelList);
        msg->byteArray32(doc.toJson());
        QMetaObject::invokeMethod(client, "sendMessage", Qt::QueuedConnection,
                                  Q_ARG(NetworkMessage*, static_cast<NetworkMessage*>(msg)), Q_ARG(bool, true));
    }
}*/

void ServerManager::insertField(QString key, QVariant value, bool erase)
{
    if(!m_parameters.contains(key) || erase)
    {
        m_parameters.insert(key, value);
    }
}
QVariant ServerManager::getValue(QString key) const
{
    if(m_parameters.contains(key))
    {
        return m_parameters[key];
    }
    return QVariant();
}
ServerManager::ServerState ServerManager::state() const
{
    return m_state;
}
ChannelModel* ServerManager::channelModel() const
{
    return m_model.get();
}

const QHash<QTcpSocket*, TcpClient*> ServerManager::connections() const
{
    return m_connections;
}

void ServerManager::setState(const ServerManager::ServerState& state)
{
    if(m_state == state)
        return;

    m_state= state;
    emit stateChanged(m_state);
}

void ServerManager::quit()
{
    if(!sender())
        return;

    closeAllConnections();
    stopListening();
}

void ServerManager::accept(qintptr handle, TcpClient* connection, QThread* thread)
{
    Q_UNUSED(thread)
    if(nullptr == connection)
        return;

    emit errorOccured(tr("New Incoming Connection!"), LogController::Info);

    connect(connection, &TcpClient::dataReceived, this, &ServerManager::messageReceived, Qt::QueuedConnection); //
    connect(connection, &TcpClient::socketInitiliazed, this, &ServerManager::initClient, Qt::QueuedConnection);

    connect(connection, &TcpClient::serverAuthFail, this, &ServerManager::sendOffAuthFail, Qt::QueuedConnection);
    connect(connection, &TcpClient::serverAuthSuccess, this, &ServerManager::sendOffAuthSuccessed,
            Qt::QueuedConnection);

    connect(connection, &TcpClient::adminAuthFailed, this, &ServerManager::sendOffAdminAuthFail, Qt::QueuedConnection);
    connect(connection, &TcpClient::adminAuthSucceed, this, &ServerManager::sendOffAdminAuthSuccessed,
            Qt::QueuedConnection);
    // connect(connection, &TcpClient::itemChanged, this, &ServerManager::sendOffModelToAll, Qt::QueuedConnection);
    connect(connection, &TcpClient::itemChanged, this,
            [this]() {
                qDebug() << "connection ItemChanged";
                // sendOffModelToAll();
            },
            Qt::QueuedConnection);

    connect(connection, &TcpClient::checkServerAcceptClient, this, &ServerManager::serverAcceptClient,
            Qt::QueuedConnection);
    connect(connection, &TcpClient::checkServerPassword, this, &ServerManager::checkAuthToServer, Qt::QueuedConnection);
    connect(connection, &TcpClient::checkAdminPassword, this, &ServerManager::checkAuthAsAdmin, Qt::QueuedConnection);
    connect(connection, &TcpClient::checkChannelPassword, this, &ServerManager::checkAuthToChannel,
            Qt::QueuedConnection);
    connect(connection, &TcpClient::channelPassword, this, &ServerManager::setChannelPassword, Qt::QueuedConnection);

    connect(connection, &TcpClient::socketDisconnection, this, &ServerManager::disconnectedUser, Qt::QueuedConnection);
    connect(connection, &TcpClient::socketError, this, &ServerManager::error, Qt::QueuedConnection);
    connection->setSocketHandleId(handle);

    // emit clientAccepted();
    QMetaObject::invokeMethod(connection, &TcpClient::startReading, Qt::QueuedConnection);
}

/*void ServerManager::sendOffModelToAll()
{
    qDebug() << "sendoffModelToAll" << m_connections.size();
    auto values= m_connections.values();
    for(auto& connection : values)
    {
        sendOffModel(connection);
    }
}*/

void ServerManager::disconnectedUser()
{
    if(!sender())
        return;

    TcpClient* client= qobject_cast<TcpClient*>(sender());
    if(!client)
        return;

    emit errorOccured(tr("User %1 has been disconnected!").arg(client->playerName()), LogController::Info);
    removeClient(client);
}

void ServerManager::removeClient(TcpClient* client)
{
    emit client->isReady();

    auto socket= client->getSocket();
    m_connections.remove(socket);

    m_model->removeChild(client->getId());
    client->disconnect();

    if(nullptr != socket)
    {
        socket->disconnect();
        if(socket->isOpen())
        {
            socket->close();
        }
        socket->deleteLater();
    }
    // sendOffModelToAll();
    client->deleteLater();
}
void ServerManager::setChannelPassword(QString chanId, QByteArray passwd)
{
    auto item= m_model->getItemById(chanId);
    if(nullptr == item)
        return;

    auto channel= dynamic_cast<Channel*>(item);
    if(nullptr == channel)
        return;

    channel->setPassword(passwd);
    // sendOffModelToAll();
}

void ServerManager::closeAllConnections()
{
    auto clients= m_connections.values();
    std::for_each(clients.begin(), clients.end(), [](TcpClient* client) {
        QMetaObject::invokeMethod(client, &TcpClient::closeConnection, Qt::QueuedConnection);
    });
}

void ServerManager::error(QAbstractSocket::SocketError socketError)
{
    if(QAbstractSocket::RemoteHostClosedError == socketError)
        return;
    if(!sender())
        return;

    TcpClient* client= qobject_cast<TcpClient*>(sender());
    if(!client)
        return;

    auto socket= client->getSocket();

    if(!socket)
        return;

    emit errorOccured(socket->errorString(), LogController::Error);
}
