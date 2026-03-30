/***************************************************************************
 *   Copyright (C) 2023 by Renaud Guezennec                                *
 *   renaud@rolisteam.org                                                  *
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

#include <QDebug>
#include <QObject>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QSignalSpy>
#include <QTest>
#include <memory>

#include "controller/applicationcontroller.h"
#include "controller/gamecontroller.h"
#include "controller/instantmessagingcontroller.h"

#include "data/chatroom.h"
#include "data/player.h"
#include "diceparser_qobject/diceroller.h"
#include "helper.h"
#include "model/chatroomsplittermodel.h"
#include "model/messagemodel.h"
#include "model/playermodel.h"
#include "test_root_path.h"
#include "worker/modelhelper.h"

#include <common_qml/theme.h>

void registerTypeTest()
{
    static bool registered= false;

    if(registered)
        return;

    Q_INIT_RESOURCE(viewsqml);
    Q_INIT_RESOURCE(textedit);
    Q_INIT_RESOURCE(rolisteam);
    Q_INIT_RESOURCE(resources);

    customization::Theme::setPath(QString("%1/resources/stylesheet/qml/theme.ini").arg(tests::root_path));
    qRegisterMetaType<PlayerModel*>("PlayerModel*");
    qRegisterMetaType<customization::Theme*>("customization::Theme*");
    qRegisterMetaType<customization::StyleSheet*>("customization::StyleSheet*");

    qmlRegisterAnonymousType<PlayerModel>("PlayerModel", 1);

    qmlRegisterSingletonType<customization::Theme>("Customization", 1, 0, "Theme",
                                                   [](QQmlEngine* engine, QJSEngine*) -> QObject*
                                                   {
                                                       auto instead= customization::Theme::instance();
                                                       engine->setObjectOwnership(instead, QQmlEngine::CppOwnership);
                                                       return instead;
                                                   });

    QQuickStyle::setStyle("rolistyle");
    QQuickStyle::setFallbackStyle("Fusion");

    registered= true;
}

class InstantMessagingTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void sendMessage();
    void sendMessage_data();

    void applicationController();

    void serializationTest();
    void chatroomModelTest();

    void imControllerTest();

private:
    std::unique_ptr<PlayerModel> m_model;
    std::unique_ptr<InstantMessagingController> m_ctrl;
    std::unique_ptr<Player> m_player1;
    std::unique_ptr<Player> m_player2;
    QString m_name1{Helper::randomString()};
    QString m_name2{Helper::randomString()};
};
void InstantMessagingTest::initTestCase()
{
    registerTypeTest();
}
void InstantMessagingTest::init()
{
    m_player1= std::make_unique<Player>(m_name1, Qt::blue, true);
    m_player2= std::make_unique<Player>(m_name2, Qt::green, false);

    m_model.reset(new PlayerModel());
    m_ctrl.reset(new InstantMessagingController(new DiceRoller(this), m_model.get()));

    // m_ctrl->setDiceParser();

    m_model->addPlayer(m_player1.get());
    m_model->addPlayer(m_player2.get());

    auto model= m_ctrl->mainModel();

    QCOMPARE(model->rowCount(), 1);
}

void InstantMessagingTest::cleanup()
{
    m_ctrl.release();
    m_model.release();
    m_player1.release();
    m_player2.release();
}

void InstantMessagingTest::sendMessage()
{
    QFETCH(QString, text);
    QFETCH(QUrl, url);
    QFETCH(int, idx);
    QFETCH(QString, name);
    QFETCH(InstantMessaging::MessageInterface::MessageType, type);

    QList<Player*> list({m_player1.get(), m_player2.get()});
    auto personId= list[idx]->uuid();

    auto global= m_ctrl->globalChatroom();
    auto model= global->messageModel();

    QSignalSpy spy(model, &InstantMessaging::MessageModel::messageAdded);

    global->addMessage(text, url, personId, name);

    spy.wait(100);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(model->rowCount(), 1);

    auto msgtype= model->data(model->index(0, 0), InstantMessaging::MessageModel::MessageTypeRole)
                      .value<InstantMessaging::MessageInterface::MessageType>();

    QCOMPARE(msgtype, type);
}
void InstantMessagingTest::sendMessage_data()
{
    QTest::addColumn<QString>("text");
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<int>("idx");
    QTest::addColumn<QString>("name");
    QTest::addColumn<InstantMessaging::MessageInterface::MessageType>("type");

    QTest::addRow("text1") << "hello there" << QUrl{} << 0 << m_name1 << InstantMessaging::MessageInterface::Text;
    QTest::addRow("text2") << "General Kenobi" << QUrl{} << 1 << m_name2 << InstantMessaging::MessageInterface::Text;

    QTest::addRow("dice1") << "!1d100" << QUrl{} << 0 << m_name1 << InstantMessaging::MessageInterface::Dice;
    QTest::addRow("dice2") << "!10d10" << QUrl{} << 1 << m_name2 << InstantMessaging::MessageInterface::Dice;

    QTest::addRow("cmd1") << "/e draws his sword" << QUrl{} << 0 << m_name1
                          << InstantMessaging::MessageInterface::Command;
    QTest::addRow("cmd2") << "/me pets the cat" << QUrl{} << 1 << m_name2
                          << InstantMessaging::MessageInterface::Command;

    QTest::addRow("error1") << "!4d10k8" << QUrl{} << 0 << m_name1 << InstantMessaging::MessageInterface::Error;
    QTest::addRow("error2") << "!4d10e[>0]" << QUrl{} << 1 << m_name2 << InstantMessaging::MessageInterface::Error;
}

void InstantMessagingTest::applicationController()
{
    GameController gameCtrl("rolisteam", QGuiApplication::clipboard());
    ApplicationController ctrl(&gameCtrl);

    auto msg1= Helper::randomString();
    auto id1= Helper::randomString();

    ctrl.msgToAll(msg1, id1);

    auto msg2= Helper::randomString();
    auto id2= Helper::randomString();
    ctrl.msgToGM(msg2, id2);

    auto msg3= Helper::randomString();
    auto id3= Helper::randomString();
    ctrl.rollDice(msg3, id3);
}

void InstantMessagingTest::serializationTest()
{
    m_ctrl->sendMessageToGM(Helper::randomString(), m_player1->uuid());
    m_ctrl->sendMessageToGlobal(Helper::randomString(), m_player1->uuid());
    m_ctrl->sendMessageToGlobal(Helper::randomString(), m_player1->uuid());
    m_ctrl->sendMessageToGlobal(Helper::randomString(), m_player1->uuid());

    auto res= ModelHelper::saveInstantMessageModel(m_ctrl->model());

    m_model->clear(true);
    ModelHelper::fetchInstantMessageModel(res, m_ctrl->model());

    QCOMPARE(m_ctrl->model()->rowCount(), 3);
}

void InstantMessagingTest::chatroomModelTest()
{
    auto rooms= m_ctrl->model();
    auto model= m_ctrl->mainModel();

    QCOMPARE(model->rowCount(model->index(0, 0)), 0);
    QCOMPARE(model->rowCount(), 1);

    model->addFilterModel(rooms, {m_player1->uuid()}, false);

    QCOMPARE(model->rowCount(), 2);

    model->mergeGlobal(m_player1->uuid(), 1);
    QCOMPARE(model->rowCount(), 1);

    auto name= Helper::randomString();
    m_ctrl->addExtraChatroom(name, true, {});
    QCOMPARE(rooms->rowCount(), 4);

    auto const& rs= rooms->rooms();
    QString id;
    for(auto const& r : rs)
    {
        if(r->title() == name)
            id= r->uuid();
    }

    model->addFilterModel(rooms, {m_player1->uuid()}, false);
    QCOMPARE(model->rowCount(), 2);

    model->cleanAll();
    QCOMPARE(model->rowCount(), 0);
    {
        name= Helper::randomString();
        m_ctrl->addExtraChatroom(name, true, {});
        QCOMPARE(rooms->rowCount(), 5);

        auto const& rs= rooms->rooms();
        for(auto const& r : rs)
        {
            if(r->title() == name)
                id= r->uuid();
        }
    }
    m_ctrl->splitScreen(id, 1);

    model->moveLeft(id, 1);
    model->moveRight(id, 0);
    model->removeChatroom(id);
}

void InstantMessagingTest::imControllerTest()
{
    m_ctrl->gmChatroom();
    m_ctrl->unread();
    m_ctrl->openLink(Helper::randomUrl().toString());
    m_ctrl->setDiceParser(nullptr);
    m_ctrl->setGameController(nullptr);
    m_ctrl->closeChatroom(Helper::randomString(), false);
    m_ctrl->closeChatroom(Helper::randomString(), true);
    m_ctrl->moveRight(Helper::randomString(), 0);
    m_ctrl->moveLeft(Helper::randomString(), 0);
    m_ctrl->mergeScreen(Helper::randomString(), 0);
    m_ctrl->mergeScreen(Helper::randomString(), 1);
    m_ctrl->resetScreen();
    m_ctrl->translateDiceResult({}, Helper::randomString());

    QHash<int, QList<int>> rollResult{{10, {1, 2, 3, 4, 5, 6, 7, 8, 9}}, {6, {1, 2, 3, 4, 5, 6}}};

    m_ctrl->translateDiceResult(rollResult, Helper::randomString());

    m_ctrl->setCurrentTab(1);
    QCOMPARE(m_ctrl->currentTab(), 1);
}

QTEST_MAIN(InstantMessagingTest);

#include "tst_im.moc"
