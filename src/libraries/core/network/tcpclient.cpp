/*************************************************************************
 *   Copyright (C) 2011 by Joseph Boudou                                 *
 *   Copyright (C) 2015 by Renaud Guezennec                              *
 *                                                                       *
 *   https://rolisteam.org/                                           *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/
#include "tcpclient.h"
#include "channel.h"

#include "worker/playermessagehelper.h"
#include <QHostAddress>
#include <QThread>

TcpClient::TcpClient(QTcpSocket* socket, QObject* parent)
    : TreeItem(parent), m_socket(socket), m_isAdmin(false), m_player(new Player)
{
    m_remainingData= 0;
    m_headerRead= 0;
    qRegisterMetaType<TcpClient::ConnectionEvent>();
}
TcpClient::~TcpClient()
{
    if(nullptr != m_stateMachine)
    {
        delete m_stateMachine;
        m_stateMachine= nullptr;
    }
}

void TcpClient::resetStateMachine()
{
    if(nullptr == m_socket)
        return;

    m_stateMachine= new QStateMachine();

    connect(m_stateMachine, SIGNAL(started()), this, SIGNAL(isReady()));
    m_incomingConnection= new QState();
    m_controlConnection= new QState();
    m_authentificationServer= new QState();
    m_disconnected= new QState();

    m_connected= new QStateMachine();
    m_inChannel= new QState();
    m_wantToGoToChannel= new QState();

    m_stateMachine->addState(m_incomingConnection);
    m_stateMachine->setInitialState(m_incomingConnection);
    m_stateMachine->addState(m_controlConnection);
    m_stateMachine->addState(m_authentificationServer);
    m_stateMachine->addState(m_connected);
    m_stateMachine->addState(m_disconnected);

    m_connected->addState(m_inChannel);
    m_connected->setInitialState(m_inChannel);
    m_connected->addState(m_wantToGoToChannel);

    m_stateMachine->start();

    connect(m_incomingConnection, &QState::activeChanged, this, [=](bool b) {
        qDebug() << "incomming state";
        if(b)
        {
            m_currentState= m_incomingConnection;
        }
    });
    connect(m_controlConnection, &QState::activeChanged, this, [=](bool b) {
        qDebug() << "control state";
        if(b)
        {
            m_currentState= m_controlConnection;
            emit checkServerAcceptClient(this);
        }
    });

    connect(m_authentificationServer, &QState::activeChanged, this, [=](bool b) {
        qDebug() << "authentification state";
        if(b)
        {
            m_currentState= m_authentificationServer;
            if(m_knownUser)
            {
                emit checkServerPassword(this);
            }
            else
            {
                m_waitingData= true;
            }
        }
    });
    connect(m_wantToGoToChannel, &QState::activeChanged, this, [=](bool b) {
        if(b)
        {
            m_currentState= m_wantToGoToChannel;
        }
    });

    connect(m_disconnected, &QState::activeChanged, this, [=](bool b) {
        if(b)
        {
            m_currentState= m_disconnected;
            m_forwardMessage= false;
            if(nullptr != m_socket)
            {
                m_socket->close();
            }
        }
    });

    connect(m_connected, &QState::activeChanged, this, [=](bool b) {
        if(b)
        {
            m_forwardMessage= true;
        }
    });

    m_incomingConnection->addTransition(this, &TcpClient::checkSuccess, m_controlConnection);
    m_incomingConnection->addTransition(this, &TcpClient::checkFail, m_disconnected);
    m_incomingConnection->addTransition(this, &TcpClient::protocolViolation, m_disconnected);

    m_controlConnection->addTransition(this, &TcpClient::controlSuccess, m_authentificationServer);
    m_controlConnection->addTransition(this, &TcpClient::controlFail, m_disconnected);
    m_controlConnection->addTransition(this, &TcpClient::protocolViolation, m_disconnected);

    m_authentificationServer->addTransition(this, &TcpClient::serverAuthSuccess, m_connected);
    m_authentificationServer->addTransition(this, &TcpClient::serverAuthFail, m_disconnected);
    m_authentificationServer->addTransition(this, &TcpClient::protocolViolation, m_disconnected);

    m_connected->addTransition(this, &TcpClient::socketDisconnection, m_disconnected);
    m_connected->addTransition(this, &TcpClient::protocolViolation, m_disconnected);

    m_wantToGoToChannel->addTransition(this, &TcpClient::channelAuthFail, m_inChannel);
    m_wantToGoToChannel->addTransition(this, &TcpClient::channelAuthSuccess, m_inChannel);
    m_inChannel->addTransition(this, &TcpClient::moveChannel, m_wantToGoToChannel);

    emit socketInitiliazed();
}

void TcpClient::startReading()
{
    m_socket= new QTcpSocket();
    connect(m_socket, &QTcpSocket::disconnected, this, &TcpClient::socketDisconnection);
    connect(m_socket, &QTcpSocket::readyRead, this, &TcpClient::receivingData);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred), this,
            &TcpClient::connectionError);

    m_socket->setSocketDescriptor(getSocketHandleId());
    resetStateMachine();
}

qintptr TcpClient::getSocketHandleId() const
{
    return m_socketHandleId;
}

void TcpClient::setSocketHandleId(const qintptr& socketHandleId)
{
    m_socketHandleId= socketHandleId;
}

bool TcpClient::isAdmin() const
{
    return m_isAdmin;
}

void TcpClient::setIsAdmin(bool isAdmin)
{
    m_isAdmin= isAdmin;
}

bool TcpClient::isGM() const
{
    if(m_player == nullptr)
        return false;
    return m_player->isGM();
}

QString TcpClient::playerId() const
{
    if(nullptr != m_player)
        return m_player->uuid();

    return {};
}

QString TcpClient::playerName() const
{
    if(m_player)
        return m_player->name();

    return {};
}

void TcpClient::setInfoPlayer(NetworkMessageReader* msg)
{
    if(nullptr == msg && nullptr == m_player)
        return;

    if(PlayerMessageHelper::readPlayer(*msg, m_player.get()))
    {
        m_knownUser= true;
        setName(m_player->name());
        setId(m_player->uuid());
        emit playerInfoDefined();
    }
}

void TcpClient::fill(NetworkMessageWriter* msg)
{
    if(nullptr != m_player)
    {
        // m_player->fill(*msg);
        PlayerMessageHelper::writePlayerIntoMessage(*msg, m_player.get());
    }
}

bool TcpClient::isFullyDefined()
{
    if(nullptr != m_player)
    {
        return m_player->isFullyDefined();
    }
    return false;
}

void TcpClient::closeConnection()
{
    if(nullptr != m_socket)
    {
        m_socket->disconnectFromHost();
    }
    emit clientSaysGoodBye();
}

/*void TcpClient::addPlayerFeature(QString uuid, QString name, quint8 version)
{
    if(nullptr == m_player)
        return;

    if(m_player->uuid() == uuid)
    {
        m_player->setFeature(name, version);
    }
}*/

void TcpClient::receivingData()
{
    if(m_socket.isNull())
    {
        return;
    }
    quint32 dataRead= 0;

    while(!m_socket.isNull() && m_socket->bytesAvailable())
    {
        if(!m_receivingData)
        {
            qint64 readDataSize= 0;
            char* tmp= reinterpret_cast<char*>(&m_header);

            // To do only if there is enough data
            readDataSize= m_socket->read(tmp + m_headerRead, static_cast<qint64>(sizeof(NetworkMessageHeader))
                                                                 - static_cast<qint64>(m_headerRead));

            if(readDataSize != static_cast<qint64>(sizeof(NetworkMessageHeader))
               && readDataSize + static_cast<qint64>(m_headerRead) != static_cast<qint64>(sizeof(NetworkMessageHeader)))
            {
                m_headerRead+= static_cast<quint64>(readDataSize);
                continue;
            }
            else
            {
                m_headerRead= 0;
            }
            m_buffer= new char[m_header.dataSize];
            m_remainingData= m_header.dataSize;
            emit readDataReceived(m_header.dataSize, m_header.dataSize);
        }
        // To do only if there is enough data
        dataRead
            = m_socket->read(&(m_buffer[static_cast<int>(static_cast<quint64>(m_header.dataSize) - m_remainingData)]),
                             static_cast<qint64>(m_remainingData));
        m_dataReceivedTotal+= dataRead;

        if(dataRead < m_remainingData)
        {
            m_remainingData-= dataRead;
            m_receivingData= true;
            emit readDataReceived(m_remainingData, m_header.dataSize);
            // m_socket->waitForReadyRead();
        }
        else
        {
            m_headerRead= 0;
            dataRead= 0;
            m_dataReceivedTotal= 0;
            emit readDataReceived(0, 0);
            m_receivingData= false;
            m_remainingData= 0;
            forwardMessage();
        }
    }
}
bool TcpClient::isCurrentState(QState* state)
{
    return state == m_currentState;
}

QString TcpClient::getChannelPassword() const
{
    return m_channelPassword;
}

QString TcpClient::getAdminPassword() const
{
    return m_adminPassword;
}

QString TcpClient::getServerPassword() const
{
    return m_serverPassword;
}
void TcpClient::forwardMessage()
{
    QByteArray array(reinterpret_cast<char*>(&m_header), sizeof(NetworkMessageHeader));
    array.append(m_buffer, static_cast<int>(m_header.dataSize));
    if(!isCurrentState(m_disconnected))
    {
        if(m_header.category == NetMsg::AdministrationCategory)
        {
            NetworkMessageReader msg;
            msg.setData(array);
            readAdministrationMessages(msg);
            emit dataReceived(array);
        }
        else if(!m_forwardMessage)
        {
            delete[] m_buffer;
            emit protocolViolation();
        }
        else
        {
            emit dataReceived(array);
        }
    }
}

void TcpClient::sendMessage(NetworkMessage* msg, bool deleteMsg)
{
    if((nullptr != m_socket) && (m_socket->isWritable()))
    {
        NetworkMessageHeader* data= msg->buffer();
        qint64 dataSend= m_socket->write(reinterpret_cast<char*>(data), data->dataSize + sizeof(NetworkMessageHeader));
        if(-1 == dataSend)
        {
            if(m_socket->state() != QAbstractSocket::ConnectedState)
            {
                emit socketDisconnection();
            }
        }
    }
    if(deleteMsg)
    {
        delete msg;
    }
}
void TcpClient::connectionError(QAbstractSocket::SocketError error)
{
    emit socketError(error);
    if(nullptr != m_socket)
    {
        qWarning() << m_socket->errorString() << error;
    }
}

void TcpClient::sendEvent(TcpClient::ConnectionEvent event)
{
    if(nullptr != m_player)
        qDebug() << "server connection to " << m_player->name() << "recieve event:" << event;
    switch(event)
    {
    case CheckSuccessEvent:
        emit checkSuccess();
        break;
    case CheckFailEvent:
        emit checkFail();
        break;
    case ControlFailEvent:
        emit controlFail();
        break;
    case ControlSuccessEvent:
        emit controlSuccess();
        break;
    case ServerAuthDataReceivedEvent:
        emit serverAuthDataReceived();
        break;
    case ServerAuthFailEvent:
        emit serverAuthFail();
        break;
    case ServerAuthSuccessEvent:
        emit serverAuthSuccess();
        break;
    case AdminAuthSuccessEvent:
        emit adminAuthSucceed();
        m_isAdmin= true;
        break;
    case AdminAuthFailEvent:
        emit adminAuthFailed();
        m_isAdmin= false;
        break;
    case ChannelAuthSuccessEvent:
        emit channelAuthSuccess();
        break;
    case ChannelAuthFailEvent:
        emit channelAuthFail();
        break;
    case MoveChanEvent:
        emit moveChannel();
        break;
    case ChannelChanged:
        sendOffChannelChanged();
        break;
    default:
        break;
    }
}
void TcpClient::sendOffChannelChanged()
{
    NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::MovedIntoChannel);
    sendMessage(&msg, false);
}
void TcpClient::readAdministrationMessages(NetworkMessageReader& msg)
{
    switch(msg.action())
    {
    case NetMsg::ConnectionInfo:
        m_serverPassword= msg.byteArray32();
        setName(msg.string32());
        setId(msg.string32());
        if(m_waitingData)
        {
            emit checkServerPassword(this);
        }
        m_waitingData= false;
        break;
    case NetMsg::ChannelPassword:
        if(isAdmin())
        {
            auto channelId= msg.string8();
            auto passwd= msg.byteArray32();
            emit channelPassword(channelId, passwd);
        }
        break;
    case NetMsg::MoveChannel:
    {
        m_wantedChannel= msg.string32();
        auto passwd= msg.byteArray32();
        emit checkChannelPassword(this, m_wantedChannel, passwd);
    }
    break;
    case NetMsg::AdminPassword:
        m_adminPassword= msg.byteArray32();
        emit checkAdminPassword(this);
        break;
    case NetMsg::Goodbye:
        closeConnection();
        break;
    default:
        break;
    }
}

Channel* TcpClient::getParentChannel() const
{
    return dynamic_cast<Channel*>(getParentItem());
}

void TcpClient::setParentChannel(Channel* parent)
{
    setParentItem(parent);
}
QTcpSocket* TcpClient::getSocket()
{
    return m_socket;
}
int TcpClient::indexOf(TreeItem*)
{
    return -1;
}

void TcpClient::readFromJson(QJsonObject& json)
{
    setName(json["name"].toString());
    setId(json["id"].toString());
    setIsAdmin(json["admin"].toBool());
    auto playerId= json["idPlayer"].toString();
    if(m_player)
        m_player->setUuid(playerId);
}

void TcpClient::writeIntoJson(QJsonObject& json)
{
    json["type"]= "client";
    json["name"]= m_name;
    json["gm"]= isGM();
    json["admin"]= m_isAdmin;
    json["id"]= m_id;
    json["idPlayer"]= playerId();
}
QString TcpClient::getIpAddress()
{
    if(nullptr != m_socket)
    {
        return m_socket->peerAddress().toString();
    }
    return {};
}

QString TcpClient::getWantedChannel()
{
    return m_wantedChannel;
}
bool TcpClient::isConnected() const
{
    if(!m_socket.isNull())
        return (m_socket->isValid() & (m_socket->state() == QAbstractSocket::ConnectedState));
    else
        return false;
}
