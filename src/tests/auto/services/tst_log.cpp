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

#include "common/include/common/logcontroller.h"
#include <QtDebug>

class LogTest : public QObject
{
    Q_OBJECT

public:
    LogTest();

private slots:
    void logTest();
};

LogTest::LogTest() {}

void LogTest::logTest()
{
    LogController logCtrl(true);

    qInfo() << "Info";
    qCritical() << "Critical";

    auto mode = logCtrl.currentModes();
    Q_UNUSED(mode);

    logCtrl.setLogLevel(LogController::Warning);
    logCtrl.setLogLevel(LogController::Info);
    logCtrl.setLogLevel(LogController::Debug);

    logCtrl.setSignalInspection(true);


}


QTEST_MAIN(LogTest);

#include "tst_log.moc"
