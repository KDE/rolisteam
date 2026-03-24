#include "network/serverconnectionmanager.h"
#include "common/logcategory.h"
#include "media/networktype.h"
#include "network/channelmodel.h"
#include "network/ipbanaccepter.h"
#include "network/iprangeaccepter.h"
#include "network/messagedispatcher.h"
#include "network/networkmessagewriter.h"
#include "network/passwordaccepter.h"
#include "network/serverconnection.h"
#include "worker/playermessagehelper.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QReadLocker>
#include <QTimer>
#include <QWriteLocker>

void sendEventToClient(ServerConnection* client, ServerConnection::ConnectionEvent event)
{
    QMetaObject::invokeMethod(client, "sendEvent", Qt::QueuedConnection,
                              Q_ARG(ServerConnection::ConnectionEvent, event));
}

ServerConnectionManager::ServerConnectionManager(const QMap<QString, QVariant>& parameters, QObject* parent)
    : QObject(parent), m_model(new ChannelModel(true)), m_parameters(parameters)
{
    int chCount= parameters.value(network::configkeys::channelCount, 1).toInt();
    int count= m_model->rowCount(QModelIndex());
    for(int i= count; i < chCount; ++i)
    {
        m_model->addChannel(QString(), QStringLiteral("Channel %1").arg(i), QString(), {}, QString());
    }

    qRegisterMetaType<NetworkMessage*>("NetworkMessage*");

    connect(m_model.get(), &ChannelModel::totalSizeChanged, this, &ServerConnectionManager::memoryChannelChanged);

    m_msgDispatcher= new MessageDispatcher(this);
    connect(this, &ServerConnectionManager::messageMustBeDispatched, m_msgDispatcher,
            &MessageDispatcher::dispatchMessage, Qt::QueuedConnection);

    connect(m_msgDispatcher, &MessageDispatcher::messageForAdmin, this, &ServerConnectionManager::processMessageAdmin);

    m_corEndProcess.reset(new PasswordAccepter());
    m_tcpConnectionAccepter.reset(new IpBanAccepter());
    m_tcpConnectionAccepter->setNext(new IpRangeAccepter());
    m_adminAccepter.reset(new PasswordAccepter(PasswordAccepter::Admin));

    connect(
        m_model.get(), &ChannelModel::userHasJoinedChannel, this,
        [this](const QString& channelId, const QString& userId)
        {
            auto channel= dynamic_cast<Channel*>(m_model->getItemById(channelId));
            auto conn= dynamic_cast<ServerConnection*>(m_model->getItemById(userId));
            if(!channel || !conn)
                return;

            if(conn->isFullyDefined())
                channel->updateNewClient(conn);
        },
        Qt::QueuedConnection);
}

ServerConnectionManager::~ServerConnectionManager()
{
    // stopListening();
}

int ServerConnectionManager::countConnection() const
{
    QReadLocker locker(&m_lock);
    return m_connections.count();
}

void ServerConnectionManager::messageReceived(QByteArray array)
{
    ServerConnection* client= qobject_cast<ServerConnection*>(sender());
    if(nullptr == client)
        return;

    emit messageMustBeDispatched(array, client->getParentChannel(), client);
}

void ServerConnectionManager::initClient()
{
    ServerConnection* client= qobject_cast<ServerConnection*>(sender());
    if(nullptr != client)
    {
        m_connections.insert(client->getSocket(), client);
        sendEventToClient(client, ServerConnection::CheckSuccessEvent);
    }
    else
    {
        sendEventToClient(client, ServerConnection::CheckFailEvent);
    }
}

/////////////////////////////////////////////////////////
///
/// Slot to perform check during connection process.
///
////////////////////////////////////////////////////////
void ServerConnectionManager::serverAcceptClient(ServerConnection* client)
{
    if(nullptr == client)
    {
        qCDebug(ServerLogCat) << "client is null";
        return;
    }

    QMap<QString, QVariant> data(m_parameters);
    data["currentIp"]= client->getIpAddress();
    if(m_tcpConnectionAccepter->runAccepter(data))
    {
        sendEventToClient(client, ServerConnection::ControlSuccessEvent);
    }
    else
    {
        sendEventToClient(client, ServerConnection::ControlFailEvent);
    }
}

void ServerConnectionManager::checkAuthToServer(ServerConnection* client)
{
    qCDebug(ServerLogCat) << "check auth to server";
    if(nullptr == client)
        return;
    qCDebug(ServerLogCat) << "cilent is fully defined" << client->isFullyDefined();

    QMap<QString, QVariant> data(m_parameters);
    data["currentIp"]= client->getIpAddress();
    data["userpassword"]= client->getServerPassword();

    if(m_corEndProcess->runAccepter(data))
    {
        m_model->addConnectionToChannel(m_model->defaultChannelId(), client);
        sendEventToClient(client, ServerConnection::ServerAuthSuccessEvent);
        qCDebug(ServerLogCat) << "server auth successed";
    }
    else
    {
        sendEventToClient(client, ServerConnection::ServerAuthFailEvent);
        qCDebug(ServerLogCat) << "server auth failed";
    }
}

void ServerConnectionManager::checkAuthAsAdmin(ServerConnection* client)
{
    QMap<QString, QVariant> data(m_parameters);
    data["userpassword"]= client->getAdminPassword();
    if(m_adminAccepter->runAccepter(data))
    {
        qDebug() << "[Admin] password correct";
        sendEventToClient(client, ServerConnection::AdminAuthSuccessEvent);
    }
    else
    {
        qDebug() << "[Admin] password incorrect";
        sendEventToClient(client, ServerConnection::AdminAuthFailEvent);
    }
}

void ServerConnectionManager::memoryChannelChanged(quint64 size)
{
    if(size > m_parameters[network::configkeys::memorySize].toULongLong())
    {
        m_model->emptyChannelMemory();
    }
}

void ServerConnectionManager::checkAuthToChannel(ServerConnection* client, QString channelId, QByteArray password)
{
    QMap<QString, QVariant> data(m_parameters);
    auto item= m_model->getItemById(channelId);
    auto channel= dynamic_cast<Channel*>(item);

    auto eventToSend= ServerConnection::ChannelAuthSuccessEvent;

    if(nullptr == channel)
    {
        eventToSend= ServerConnection::ChannelAuthFailEvent;
        sendEventToClient(client, eventToSend);
        return;
    }

    if(channel->password() != password)
        eventToSend= ServerConnection::ChannelAuthFailEvent;

    if((m_corEndProcess->runAccepter(data)) && (eventToSend != ServerConnection::ChannelAuthFailEvent))
    {
        if(!m_model->addConnectionToChannel(channelId, client))
        {
            m_model->addConnectionToChannel(m_model->defaultChannelId(), client);
        }
    }
    else
    {
        eventToSend= ServerConnection::ChannelAuthFailEvent;
    }
    sendEventToClient(client, eventToSend);
}

/////////////////////////////////////////////////////////
///
/// Slot to perform check during connection process.
///
////////////////////////////////////////////////////////
void ServerConnectionManager::sendOffAdminAuthSuccessed()
{
    ServerConnection* client= qobject_cast<ServerConnection*>(sender());
    if(!client)
        return;

    NetworkMessageWriter* msg= new NetworkMessageWriter(NetMsg::AdministrationCategory, NetMsg::AdminAuthSucessed);
    QMetaObject::invokeMethod(client, "sendMessage", Qt::QueuedConnection,
                              Q_ARG(NetworkMessage*, static_cast<NetworkMessage*>(msg)), Q_ARG(bool, true));
}

void ServerConnectionManager::sendOffAdminAuthFail()
{
    ServerConnection* client= qobject_cast<ServerConnection*>(sender());
    if(nullptr != client)
    {
        NetworkMessageWriter* msg= new NetworkMessageWriter(NetMsg::AdministrationCategory, NetMsg::AdminAuthFail);
        QMetaObject::invokeMethod(client, "sendMessage", Qt::QueuedConnection,
                                  Q_ARG(NetworkMessage*, static_cast<NetworkMessage*>(msg)), Q_ARG(bool, true));
        emit eventOccured(
            tr("Authentification as Admin fails: %2 - %1, Wrong password.").arg(client->name(), client->getIpAddress()),
            LogController::Info);
    }
}

void ServerConnectionManager::sendOffAuthSuccessed()
{
    ServerConnection* client= qobject_cast<ServerConnection*>(sender());
    if(nullptr != client)
    {
        NetworkMessageWriter* msg
            = new NetworkMessageWriter(NetMsg::AdministrationCategory, NetMsg::AuthentificationSucessed);
        QMetaObject::invokeMethod(client, "sendMessage", Qt::QueuedConnection,
                                  Q_ARG(NetworkMessage*, static_cast<NetworkMessage*>(msg)), Q_ARG(bool, true));
        // sendOffModel(client);
    }
}

void ServerConnectionManager::sendOffAuthFail()
{
    ServerConnection* client= qobject_cast<ServerConnection*>(sender());
    if(nullptr != client)
    {
        NetworkMessageWriter* msg
            = new NetworkMessageWriter(NetMsg::AdministrationCategory, NetMsg::AuthentificationFail);
        QMetaObject::invokeMethod(client, "sendMessage", Qt::QueuedConnection,
                                  Q_ARG(NetworkMessage*, static_cast<NetworkMessage*>(msg)), Q_ARG(bool, true));
        emit eventOccured(tr("Authentification fails: %1 try to connect to the server with wrong password.")
                              .arg(client->getIpAddress()),
                          LogController::Info);
    }
}

void ServerConnectionManager::kickClient(QString id, bool isAdmin, QString senderId)
{
    auto keys= m_connections.keys();
    for(auto& key : keys)
    {
        auto value= m_connections[key];
        if(value && value->uuid() == id)
        {
            emit eventOccured(tr("User has been kick out: %2 - %1.").arg(value->name(), value->getIpAddress()),
                              LogController::Info);
        }
    }

    m_model->kick(id, isAdmin, senderId);
}

void ServerConnectionManager::banClient(QString id, bool isAdmin, QString senderId)
{
    auto keys= m_connections.keys();
    for(auto& key : keys)
    {
        auto value= m_connections[key];
        if(value && value->uuid() == id)
        {
            emit eventOccured(tr("User has been ban: %2 - %1.").arg(value->name(), value->getIpAddress()),
                              LogController::Info);
            emit banIp(value->getIpAddress());
            break;
        }
    }

    m_model->kick(id, isAdmin, senderId);
}

void ServerConnectionManager::processMessageAdmin(NetworkMessageReader* msg, Channel* chan, ServerConnection* tcp)
{
    if(tcp == nullptr)
        return;

    bool isAdmin= tcp->isAdmin();
    bool isGM= tcp->isGM();
    auto sourceId= tcp->playerId();
    switch(msg->action())
    {
    case NetMsg::KickUser:
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
            m_model->renameChannel(idChan, newName);
        }
    }
    break;
    case NetMsg::AddChannel:
    {
        if(isAdmin)
        {
            QString idparent= msg->string8();
            auto json= PlayerMessageHelper::readChannelInMsg(*msg);
            QMetaObject::invokeMethod(m_model.get(), &ChannelModel::addChannel, Qt::QueuedConnection,
                                      json["id"].toString(), json["name"].toString(), json["desc"].toString(),
                                      QByteArray(), idparent);
        }
    }
    break;
    case NetMsg::JoinChannel:
    {
        QString channelId= msg->string8();
        QString userId= msg->string8();
        auto user= m_model->getItemById(userId);
        if(user)
        {
            auto origin= dynamic_cast<Channel*>(user->getParentItem());
            TreeItem* item= m_model->getItemById(channelId);
            Channel* dest= dynamic_cast<Channel*>(item);
            if(nullptr != dest && !dest->locked() && origin != dest)
            {
                QMetaObject::invokeMethod(m_model.get(), &ChannelModel::moveClient, Qt::QueuedConnection, origin,
                                          userId, dest);
            }
        }
    }
    break;
    case NetMsg::SetDefaultChannel:
    {
        if(isAdmin)
        {
            QString channelId= msg->string8();
            QMetaObject::invokeMethod(m_model.get(), &ChannelModel::setDefaultChannelId, Qt::QueuedConnection,
                                      channelId);
        }
    }
    break;
    case NetMsg::SetChannelList:
    {
        qCDebug(ServerLogCat) << "Server received channellist";
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
        break;
    case NetMsg::AdminPassword:
        qDebug() << "[Admin] AdminPassword";
        checkAuthAsAdmin(tcp);
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

ChannelModel* ServerConnectionManager::channelModel() const
{
    return m_model.get();
}

const QHash<QTcpSocket*, ServerConnection*> ServerConnectionManager::connections() const
{
    return m_connections;
}

void ServerConnectionManager::quit()
{
    if(!sender())
        return;

    auto sockets= m_connections.keys();
    std::for_each(sockets.begin(), sockets.end(), [this](QTcpSocket* socket) { removeSocket(socket); });

    m_connections.clear();

    emit finished();
}

void ServerConnectionManager::accept(qintptr handle, ServerConnection* connection)
{
    if(nullptr == connection)
        return;

    emit eventOccured(tr("New Incoming Connection!"), LogController::Info);

    connect(connection, &ServerConnection::dataReceived, this, &ServerConnectionManager::messageReceived,
            Qt::QueuedConnection);
    connect(connection, &ServerConnection::socketInitiliazed, this, &ServerConnectionManager::initClient,
            Qt::QueuedConnection);

    connect(connection, &ServerConnection::serverAuthFail, this, &ServerConnectionManager::sendOffAuthFail,
            Qt::QueuedConnection);
    connect(connection, &ServerConnection::serverAuthSuccess, this, &ServerConnectionManager::sendOffAuthSuccessed,
            Qt::QueuedConnection);

    connect(connection, &ServerConnection::adminAuthFailed, this, &ServerConnectionManager::sendOffAdminAuthFail,
            Qt::QueuedConnection);
    connect(connection, &ServerConnection::adminAuthSucceed, this, &ServerConnectionManager::sendOffAdminAuthSuccessed,
            Qt::QueuedConnection);

    connect(
        connection, &ServerConnection::itemChanged, this, []() { qCDebug(ServerLogCat) << "connection ItemChanged"; },
        Qt::QueuedConnection);

    connect(connection, &ServerConnection::checkServerAcceptClient, this, &ServerConnectionManager::serverAcceptClient,
            Qt::QueuedConnection);
    connect(connection, &ServerConnection::checkServerPassword, this, &ServerConnectionManager::checkAuthToServer,
            Qt::QueuedConnection);
    connect(connection, &ServerConnection::checkAdminPassword, this, &ServerConnectionManager::checkAuthAsAdmin,
            Qt::QueuedConnection);
    connect(connection, &ServerConnection::checkChannelPassword, this, &ServerConnectionManager::checkAuthToChannel,
            Qt::QueuedConnection);
    connect(connection, &ServerConnection::channelPassword, this, &ServerConnectionManager::setChannelPassword,
            Qt::QueuedConnection);

    connect(connection, &ServerConnection::socketDisconnection, this, &ServerConnectionManager::disconnectedUser,
            Qt::QueuedConnection);
    connect(connection, &ServerConnection::socketError, this, &ServerConnectionManager::error, Qt::QueuedConnection);
    connection->setSocketHandleId(handle);

    // emit clientAccepted();
    QMetaObject::invokeMethod(connection, &ServerConnection::startReading, Qt::QueuedConnection);
}

void ServerConnectionManager::disconnectedUser()
{
    if(!sender())
        return;

    ServerConnection* client= qobject_cast<ServerConnection*>(sender());
    if(!client)
        return;

    emit eventOccured(tr("User %1 has been disconnected!").arg(client->playerName()), LogController::Info);
    // removeClient(client);
}

void ServerConnectionManager::removeSocket(QTcpSocket* socket)
{
    if(!socket || !m_connections.contains(socket))
        return;

    qCDebug(ServerLogCat) << this << "removing socket = " << socket;

    if(socket->isOpen())
    {
        qCDebug(ServerLogCat) << this << "socket is open, attempting to close it " << socket;
        QMetaObject::invokeMethod(socket, &QTcpSocket::close);
    }

    qCDebug(ServerLogCat) << this << "deleting socket" << socket;
    auto client= m_connections.value(socket);
    m_connections.remove(socket);

    QMetaObject::invokeMethod(socket, &QTcpSocket::deleteLater);
    client->deleteLater();

    qCDebug(ServerLogCat) << this << "client count = " << m_connections.count();
}

void ServerConnectionManager::setChannelPassword(QString chanId, QByteArray passwd)
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

void ServerConnectionManager::error(QAbstractSocket::SocketError socketError)
{
    if(QAbstractSocket::RemoteHostClosedError == socketError)
        return;
    if(!sender())
        return;

    ServerConnection* client= qobject_cast<ServerConnection*>(sender());
    if(!client)
        return;

    auto socket= client->getSocket();
    if(!socket)
        return;

    emit eventOccured(socket->errorString(), LogController::Error);
}
