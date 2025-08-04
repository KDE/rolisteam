/*************************************************************************
 *        Copyright (C) 2025 by Renaud Guezennec                         *
 *                                                                       *
 *        https://rolisteam.org/                                         *
 *                                                                       *
 *   rolisteam is free software; you can redistribute it and/or modify   *
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
#include "websocketserver.h"
#include <QWebSocket>

namespace {
constexpr auto show{"show"};
constexpr auto hide{"hide"};
}
WebSocketServer::WebSocketServer(QObject* parent) : QObject{parent},
    m_server("Rolisteam",QWebSocketServer::NonSecureMode)
{
    connect(&m_server, &QWebSocketServer::newConnection, this, [this](){
        QWebSocket *pSocket = m_server.nextPendingConnection();

        //connect(pSocket, &QWebSocket::binaryMessageReceived, this, &EchoServer::processBinaryMessage);
        connect(pSocket, &QWebSocket::textMessageReceived, this, [this](const QString& message){
            if(message.contains(show))
                emit showWindow();
            else if(message.contains(hide))
                emit hideWindow();
        });

        connect(pSocket, &QWebSocket::disconnected, this, [this](){
            QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
            if (pClient) {
                m_clients.removeAll(pClient);
                pClient->deleteLater();
            }
        });

        m_clients << pSocket;
        });
}

void WebSocketServer::startConnection()
{
    if(m_portws < 0)
        return;
    if(!m_server.listen(QHostAddress::Any, m_portws))
        qDebug() << "server can't listen";
}

int WebSocketServer::portws() const
{
    return m_portws;
}

void WebSocketServer::setPortws(int newPortws)
{
    if(m_portws == newPortws)
        return;
    m_portws= newPortws;
    emit portwsChanged();
}
