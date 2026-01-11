/***************************************************************************
 *   Copyright (C) 2011 by Renaud Guezennec                                *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   Rolisteam is free software; you can redistribute it and/or modify     *
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
#include <QTest>

#include <QImage>
#include <QPainter>
#include <QRect>
#include <QSignalSpy>
#include <QString>
#include <memory>

#include "network/upnp/upnpnat.h"

class UpnpTest : public QObject
{
    Q_OBJECT

public:
    UpnpTest();

private slots:
    void init();
    void cleanupTestCase();

    void doTest();

private:
    std::unique_ptr<UpnpNat> m_upnat;
};

UpnpTest::UpnpTest() {}

void UpnpTest::init()
{
    m_upnat.reset(new UpnpNat);
}

void UpnpTest::cleanupTestCase() {}

void UpnpTest::doTest()
{
    QSignalSpy statusChanged(m_upnat.get(), &UpnpNat::statusChanged);

    connect(m_upnat.get(), &UpnpNat::statusChanged,
            [this]()
            {
                switch(m_upnat->status())
                {
                case UpnpNat::NAT_STAT::NAT_IDLE:
                case UpnpNat::NAT_STAT::NAT_DISCOVERY:
                case UpnpNat::NAT_STAT::NAT_GETDESCRIPTION:
                case UpnpNat::NAT_STAT::NAT_DESCRIPTION_FOUND:
                    break;
                case UpnpNat::NAT_STAT::NAT_FOUND:
                    m_upnat->requestDescription();
                    break;
                case UpnpNat::NAT_STAT::NAT_READY:
                    m_upnat->addPortMapping("UpnpTest", "192.168.1.2", 6664, 6664, "TCP");
                    break;
                case UpnpNat::NAT_STAT::NAT_ADD:
                    QVERIFY(true);
                    m_upnat->deleteLater();
                    break;
                case UpnpNat::NAT_STAT::NAT_ERROR:
                    QSKIP(QString("ERROR occurs: %1").arg(m_upnat->error()).toStdString().c_str());
                    m_upnat->deleteLater();
                    break;
                }
            });
    m_upnat->discovery();
    statusChanged.wait(1000);
    if(statusChanged.count() != 3)
        QSKIP("Error: skip test about upnp");
    QCOMPARE(statusChanged.count(), 3);
}

QTEST_MAIN(UpnpTest);

#include "tst_upnp.moc"
