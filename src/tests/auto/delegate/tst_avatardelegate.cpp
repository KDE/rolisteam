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

#include <QSignalSpy>
#include <QTreeView>
#include <QUrl>

#include "data/character.h"
#include "data/player.h"
#include "helper.h"
#include "model/playermodel.h"
#include "qml_components/include/qml_components/avatarprovider.h"
#include "rwidgets/delegates/avatardelegate.h"
#include <memory>

class AvatarDelegateTest : public QObject
{
    Q_OBJECT

public:
    AvatarDelegateTest();

private slots:
    void init();
    void cleanupTestCase();

    void avatarProviderTest();
    void paintTest();

private:
    std::unique_ptr<AvatarDelegate> m_delegate;
};

AvatarDelegateTest::AvatarDelegateTest() {}

void AvatarDelegateTest::init()
{
    m_delegate.reset(new AvatarDelegate());
}

void AvatarDelegateTest::paintTest()
{
#ifdef FULL_TEST
    PlayerModel playerModel;
    QTreeView view;
    auto p1= new Player(Helper::randomString(), QColor(Qt::red), true);
    p1->setAvatar(Helper::imageData(true));

    auto p2= new Player(Helper::randomString(), QColor(Qt::red), true);
    p2->setAvatar(Helper::imageData(true));

    playerModel.addPlayer(p1);
    playerModel.addPlayer(p2);
    view.setItemDelegateForColumn(0, m_delegate.get());

    view.show();
#endif
}

void AvatarDelegateTest::cleanupTestCase() {}

void AvatarDelegateTest::avatarProviderTest()
{
    PlayerModel playerModel;

    auto p1= new Player(Helper::randomString(), QColor(Qt::red), true);
    p1->setAvatar(Helper::imageData(true));

    auto p2= new Player(Helper::randomString(), QColor(Qt::red), true);
    p2->setAvatar(Helper::imageData(true));
    auto id1= p1->uuid();
    auto id2= p2->uuid();

    playerModel.addPlayer(p1);
    playerModel.addPlayer(p2);

    AvatarProvider provider(&playerModel);

    QSize size;
    QSize requested{64, 64};
    auto img1= provider.requestImage(id1, &size, requested);
    QVERIFY(!img1.isNull());
    auto img2= provider.requestImage(id2, &size, requested);
    QVERIFY(!img2.isNull());

    QSize requested2{-1, -1};
    provider.requestImage(id2, &size, requested2);
}

QTEST_MAIN(AvatarDelegateTest);

#include "tst_avatardelegate.moc"
