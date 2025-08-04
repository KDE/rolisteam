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
#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include <QObject>
#include <QWebSocketServer>

class WebSocketServer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int portws READ portws WRITE setPortws NOTIFY portwsChanged FINAL)
public:
    explicit WebSocketServer(QObject* parent= nullptr);

    int portws() const;
    void setPortws(int newPortws);

public slots:
    void startConnection();

signals:
    void showWindow();
    void hideWindow();
    void portwsChanged();

private:
    QWebSocketServer m_server;
    QList<QWebSocket*> m_clients;
    int m_portws{-1};
};

#endif // WEBSOCKETSERVER_H
