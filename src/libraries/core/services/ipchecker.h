/*************************************************************************
 *   Copyright (C) 2013 by Renaud Guezennec                              *
 *                                                                       *
 *   https://rolisteam.org/                                           *
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
#ifndef IPCHECKER_H
#define IPCHECKER_H

#include <QNetworkAccessManager>
#include <QObject>
#include <QString>
#include <memory>

/**
 * @brief The IpChecker class is dedicated to reach https://rolisteam.org/ip.php to read the public ip.
 */
class IpChecker : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString ipAddress READ ipAddress NOTIFY ipAddressChanged)
public:
    /**
     * @brief IpChecker
     * @param parent
     */
    explicit IpChecker(QObject* parent= nullptr);
    QString ipAddress() const;

signals:
    /**
     * @brief finished
     */
    void ipAddressChanged(QString);

public slots:
    /**
     * @brief startCheck
     */
    void startCheck();

private slots:
    /**
     * @brief readText
     * @param p
     */
    void readText(QNetworkReply* p);

private:
    std::unique_ptr<QNetworkAccessManager> m_manager;
    QString m_ip;
};

#endif // IPCHECKER_H
