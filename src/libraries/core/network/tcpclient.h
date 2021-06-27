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
#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QPointer>
#include <QState>
#include <QStateMachine>
#include <QTcpSocket>

#include "channelmodel.h"

#include "networkmessage.h"
#include "treeitem.h"

#include "data/player.h"

class Channel;
/**
 * @brief The TcpClient class reprents connection state with client in server side.
 */
class TcpClient : public TreeItem
{
    Q_OBJECT
    Q_PROPERTY(QString playerName READ playerName NOTIFY playerNameChanged)
    Q_PROPERTY(QString playerId READ playerId NOTIFY playerIdChanged)

public:
    enum ConnectionEvent
    {
        CheckSuccessEvent,
        CheckFailEvent,
        ControlFailEvent,
        ControlSuccessEvent,
        ServerAuthDataReceivedEvent,
        ServerAuthFailEvent,
        ServerAuthSuccessEvent,
        AdminAuthFailEvent,
        AdminAuthSuccessEvent,
        ChannelAuthSuccessEvent,
        ChannelAuthFailEvent,
        ChannelAuthDataReceivedEvent,
        ChannelChanged,
        MoveChanEvent
    };
    Q_ENUM(ConnectionEvent)
    /**
     * @brief TcpClient
     * @param socket
     * @param parent
     */
    explicit TcpClient(QTcpSocket* socket, QObject* parent= nullptr);
    ~TcpClient();
    /**
     * @brief getParentChannel
     * @return
     */
    Channel* getParentChannel() const;
    /**
     * @brief setParentChannel
     * @param parent
     */
    void setParentChannel(Channel* parent);
    void readFromJson(QJsonObject& json);
    void writeIntoJson(QJsonObject& json);
    virtual int indexOf(TreeItem*);
    /**
     * @brief getSocket
     * @return
     */
    QTcpSocket* getSocket();
    void resetStateMachine();
    qintptr getSocketHandleId() const;
    void setSocketHandleId(const qintptr& socketHandleId);

    bool isGM() const;
    QString playerId() const;
    QString playerName() const;

    void setInfoPlayer(NetworkMessageReader* msg);
    void fill(NetworkMessageWriter* msg);

    // void addPlayerFeature(QString uuid, QString name, quint8 version);

    bool isFullyDefined();

    bool isAdmin() const;
    void setIsAdmin(bool isAdmin);
    bool isConnected() const;
    QString getIpAddress();
    QString getWantedChannel();
    QString getServerPassword() const;
    QString getAdminPassword() const;
    QString getChannelPassword() const;
signals:
    void readDataReceived(quint64, quint64);
    void dataReceived(QByteArray);

    void checkSuccess();
    void checkFail();

    void controlFail();
    void controlSuccess();

    void serverAuthDataReceived();
    void serverAuthFail();
    void serverAuthSuccess();

    void adminAuthFailed();
    void adminAuthSucceed();

    void channelAuthFail();
    void channelAuthSuccess();

    void moveChannel();

    void clientSaysGoodBye();
    void playerStatusChanged();

    // Signal to check
    void checkServerAcceptClient(TcpClient* client);
    void checkServerPassword(TcpClient* client);
    void checkAdminPassword(TcpClient* client);
    void checkChannelPassword(TcpClient* client, QString channelId, QByteArray password);
    void channelPassword(QString channelId, QByteArray password);

    void isReady();
    void hasNoRestriction();
    void hasRestriction();
    void socketDisconnection();
    void socketError(QAbstractSocket::SocketError);
    void socketInitiliazed();
    void protocolViolation();

    // properties signals
    void playerNameChanged();
    void playerIdChanged();
    void playerInfoDefined();
public slots:
    void receivingData();
    void forwardMessage();
    void sendMessage(NetworkMessage* msg, bool deleteMsg);
    void connectionError(QAbstractSocket::SocketError error);
    void sendEvent(TcpClient::ConnectionEvent);
    void startReading();
    void closeConnection();

protected:
    bool isCurrentState(QState* state);
    void readAdministrationMessages(NetworkMessageReader& msg);
    void sendOffChannelChanged();

private:
    QPointer<QTcpSocket> m_socket;
    NetworkMessageHeader m_header= {0, 0, 0};
    char* m_buffer= nullptr;
    quint64 m_headerRead;
    quint64 m_remainingData;

    QStateMachine* m_stateMachine= nullptr;
    QState* m_incomingConnection= nullptr;
    QState* m_controlConnection= nullptr;
    QState* m_authentificationServer= nullptr;
    QState* m_disconnected= nullptr;

    QStateMachine* m_connected= nullptr;
    QState* m_inChannel= nullptr;
    QState* m_wantToGoToChannel= nullptr;

    QState* m_currentState= nullptr;

    bool m_isAdmin= false;
    bool m_forwardMessage= false;

    bool m_waitingData= false;
    bool m_receivingData= false;
    quint32 m_dataReceivedTotal= 0;
    std::unique_ptr<Player> m_player;
    qintptr m_socketHandleId;
    QString m_wantedChannel;

    bool m_knownUser= false;
    QString m_serverPassword;
    QString m_adminPassword;
    QString m_channelPassword;
};
Q_DECLARE_METATYPE(TcpClient::ConnectionEvent)
#endif // TCPCLIENT_H
