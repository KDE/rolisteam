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
#include "instantmessaging/dicemessage.h"
#include "instantmessaging/errormessage.h"
#include "instantmessaging/messagefactory.h"
#include "instantmessaging/messageinterface.h"
#include "instantmessaging/textmessage.h"
#include "instantmessaging/textwritercontroller.h"
#include "model/chatroomsplittermodel.h"
#include "model/messagemodel.h"
#include "model/playermodel.h"
#include "test_root_path.h"
#include "worker/modelhelper.h"

#include <QAbstractItemModelTester>
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

    void propertiesSetGetTest();
    void propertiesSetGetTest_data();

    void chatRoomCountTest();
    void chatRoomCountTest_data();

    void textwriterControllerTest();
    void diceMessageTest();

    void errorMessageTest();

    void updaterTest();

private:
    std::unique_ptr<PlayerModel> m_playerModel;
    std::unique_ptr<InstantMessagingController> m_imCtrl;
    std::unique_ptr<QAbstractItemModelTester> m_tester;
    std::unique_ptr<InstantMessaging::InstantMessagingUpdater> m_updater;
    QPointer<QObject> m_server;
    std::unique_ptr<Player> m_player1;
    std::unique_ptr<Player> m_player2;
    QString m_name1{Helper::randomString()};
    QString m_name2{Helper::randomString()};
    Helper::TestMessageSender m_sender;
};
void InstantMessagingTest::initTestCase()
{
    registerTypeTest();
}
void InstantMessagingTest::init()
{
    m_player1= std::make_unique<Player>(m_name1, Qt::blue, true);
    m_player2= std::make_unique<Player>(m_name2, Qt::green, false);

    m_playerModel.reset(new PlayerModel);
    m_server= Helper::initWebServer();
    m_tester.reset(new QAbstractItemModelTester(m_playerModel.get()));
    m_imCtrl.reset(new InstantMessagingController(new DiceRoller, m_playerModel.get()));
    m_updater.reset(new InstantMessaging::InstantMessagingUpdater(m_imCtrl.get()));

    // m_imCtrl->setDiceParser();

    m_playerModel->addPlayer(m_player1.get());
    m_playerModel->addPlayer(m_player2.get());

    auto model= m_imCtrl->mainModel();

    QCOMPARE(model->rowCount(), 1);

    m_sender.clear();
    NetworkMessage::setMessageSender(&m_sender);
}

void InstantMessagingTest::cleanup()
{
    auto const& data= m_sender.messageData();
    for(auto const& msgData : data)
    {
        NetworkMessageReader msg;
        msg.setData(msgData);
        m_updater->processMessage(&msg);
    }

    m_imCtrl.release();
    m_playerModel.release();
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

    auto global= m_imCtrl->globalChatroom();
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
    m_imCtrl->sendMessageToGM(Helper::randomString(), m_player1->uuid());
    m_imCtrl->sendMessageToGlobal(Helper::randomString(), m_player1->uuid());
    m_imCtrl->sendMessageToGlobal(Helper::randomString(), m_player1->uuid());
    m_imCtrl->sendMessageToGlobal(Helper::randomString(), m_player1->uuid());

    auto res= ModelHelper::saveInstantMessageModel(m_imCtrl->model());

    m_playerModel->clear(true);
    ModelHelper::fetchInstantMessageModel(res, m_imCtrl->model());

    QCOMPARE(m_imCtrl->model()->rowCount(), 3);
}

void InstantMessagingTest::chatroomModelTest()
{
    auto rooms= m_imCtrl->model();
    auto model= m_imCtrl->mainModel();

    QCOMPARE(model->rowCount(model->index(0, 0)), 0);
    QCOMPARE(model->rowCount(), 1);

    model->addFilterModel(rooms, {m_player1->uuid()}, false);

    QCOMPARE(model->rowCount(), 2);

    model->mergeGlobal(m_player1->uuid(), 1);
    QCOMPARE(model->rowCount(), 1);

    auto name= Helper::randomString();
    m_imCtrl->addExtraChatroom(name, true, {});
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
        m_imCtrl->addExtraChatroom(name, true, {});
        QCOMPARE(rooms->rowCount(), 5);

        auto const& rs= rooms->rooms();
        for(auto const& r : rs)
        {
            if(r->title() == name)
                id= r->uuid();
        }
    }
    m_imCtrl->splitScreen(id, 1);

    model->moveLeft(id, 1);
    model->moveRight(id, 0);
    model->removeChatroom(id);
}

void InstantMessagingTest::imControllerTest()
{
    m_imCtrl->gmChatroom();
    m_imCtrl->unread();
    m_imCtrl->openLink(Helper::randomUrl().toString());
    m_imCtrl->setDiceParser(nullptr);
    m_imCtrl->setGameController(nullptr);
    m_imCtrl->closeChatroom(Helper::randomString(), false);
    m_imCtrl->closeChatroom(Helper::randomString(), true);
    m_imCtrl->moveRight(Helper::randomString(), 0);
    m_imCtrl->moveLeft(Helper::randomString(), 0);
    m_imCtrl->mergeScreen(Helper::randomString(), 0);
    m_imCtrl->mergeScreen(Helper::randomString(), 1);
    m_imCtrl->resetScreen();
    m_imCtrl->translateDiceResult({}, Helper::randomString());

    QHash<int, QList<int>> rollResult{{10, {1, 2, 3, 4, 5, 6, 7, 8, 9}}, {6, {1, 2, 3, 4, 5, 6}}};

    m_imCtrl->translateDiceResult(rollResult, Helper::randomString());

    m_imCtrl->setCurrentTab(1);
    QCOMPARE(m_imCtrl->currentTab(), 1);
}
void InstantMessagingTest::errorMessageTest()
{
    auto owner= Helper::randomString();
    auto writer= Helper::randomString();
    auto time= QDateTime::currentDateTime();
    InstantMessaging::ErrorMessage msg(owner, writer, time);

    QSignalSpy spy(&msg, &InstantMessaging::ErrorMessage::textChanged);
    auto text1= Helper::randomString();
    msg.setText(text1);
    msg.setText(text1);

    spy.wait(10);
    QCOMPARE(spy.count(), 1);

    auto text2= Helper::randomString();
    msg.setText(text2);
    spy.wait(10);
    QCOMPARE(spy.count(), 2);

    QCOMPARE(msg.text(), text2);

    QCOMPARE(msg.owner(), owner);
    QCOMPARE(msg.writer(), writer);
    QCOMPARE(msg.dateTime(), time);
}

void InstantMessagingTest::updaterTest()
{
    QStringList recipiants{Helper::randomString(), Helper::randomString(), Helper::randomString(),
                           Helper::randomString()};
    auto chatroom= new InstantMessaging::ChatRoom(m_playerModel.get(), InstantMessaging::ChatRoom::EXTRA, recipiants);
    auto chatroom2= new InstantMessaging::ChatRoom(m_playerModel.get(), InstantMessaging::ChatRoom::EXTRA, {});
    auto single= new InstantMessaging::ChatRoom(m_playerModel.get(), InstantMessaging::ChatRoom::SINGLEPLAYER);
    auto global= new InstantMessaging::ChatRoom(m_playerModel.get(), InstantMessaging::ChatRoom::GLOBAL);
    m_imCtrl->playerArrived(Helper::randomString());
    m_updater->openChat(chatroom, Helper::randomString());
    m_updater->openChat(chatroom);
    m_updater->openChat(chatroom2, Helper::randomString());
    m_updater->openChat(chatroom2);
    m_updater->openChat(single);
    m_updater->openChat(global);
    m_updater->openChat(nullptr);
    auto path= Helper::randomFilePath();

    m_updater->removeChatRoom(Helper::randomString(), Helper::randomString(), true);
    m_updater->removeChatRoom(Helper::randomString(), Helper::randomString(), false);

    m_updater->addChatRoom(nullptr, true);
    m_updater->addChatRoom(nullptr, false);
    m_updater->addChatRoom(global, false);

    m_updater->sendMessage();

    global->setTitle(Helper::randomString());

    m_updater->save(path);
    m_updater->load(path);
    m_updater->closeChat();
}
void InstantMessagingTest::propertiesSetGetTest()
{
    QFETCH(QString, propertyName);
    QFETCH(QVariant, propertyVal);

    auto meta= m_imCtrl->metaObject();

    auto proper= meta->property(meta->indexOfProperty(propertyName.toUtf8()));

    auto signal= proper.notifySignal();

    QSignalSpy spy(m_imCtrl.get(), signal);

    auto b= m_imCtrl->setProperty(propertyName.toUtf8(), propertyVal);

    QVERIFY(b);
    QCOMPARE(spy.count(), 1);
}

void InstantMessagingTest::propertiesSetGetTest_data()
{
    QTest::addColumn<QString>("propertyName");
    QTest::addColumn<QVariant>("propertyVal");

    QTest::addRow("cmd 1") << "localId" << QVariant::fromValue(QString("tarniten"));
    QTest::addRow("cmd 2") << "nightMode" << QVariant::fromValue(true);
    QTest::addRow("cmd 3") << "visible" << QVariant::fromValue(true);
}

void InstantMessagingTest::chatRoomCountTest()
{
    QFETCH(QList<Player*>, playerList);
    QFETCH(int, expected);

    auto model= m_imCtrl->mainModel();

    auto chatrooms= model->data(model->index(0, 0), InstantMessaging::ChatroomSplitterModel::FilterModelRole)
                        .value<InstantMessaging::FilterInstantMessagingModel*>();

    auto origin= chatrooms->rowCount() - 1;

    for(auto player : playerList)
        m_playerModel->addPlayer(player);

    auto c= chatrooms->rowCount();
    QCOMPARE(c, expected + origin);
}

void InstantMessagingTest::chatRoomCountTest_data()
{
    QTest::addColumn<QList<Player*>>("playerList");
    QTest::addColumn<int>("expected");

    QTest::addRow("cmd1") << QList<Player*>() << 1;

    QList<Player*> list;
    auto player= new Player();
    player->setUuid("1");
    list << player;
    QTest::addRow("cmd2") << list << 2;

    list.clear();
    player= new Player();
    player->setUuid("3");
    list << player;
    player= new Player();
    player->setUuid("2");
    list << player;
    QTest::addRow("cmd3") << list << 3;
}

void InstantMessagingTest::textwriterControllerTest()
{
    InstantMessaging::TextWriterController ctrl;

    QSignalSpy spy(&ctrl, &InstantMessaging::TextWriterController::urlChanged);
    QSignalSpy spy1(&ctrl, &InstantMessaging::TextWriterController::textComputed);
    QSignalSpy spy2(&ctrl, &InstantMessaging::TextWriterController::textChanged);
    QSignalSpy spy3(&ctrl, &InstantMessaging::TextWriterController::diceCommandChanged);

    auto str= Helper::randomString();
    ctrl.setText(str);
    ctrl.computeText();

    spy1.wait(100);
    QCOMPARE(spy1.count(), 1);

    spy1.clear();
    ctrl.setText(str);

    spy1.wait(10);
    QCOMPARE(spy1.count(), 0);

    auto url= Helper::randomUrl();
    ctrl.setText(url.toString());
    ctrl.computeText();

    spy.wait(1000);
    spy1.wait(100);

    QCOMPARE(spy.count(), 2);
    QCOMPARE(ctrl.imageLink(), QString());
    ctrl.setText("www.perdu.com");
    ctrl.computeText();

    spy.wait(10);
    spy1.wait(100);
    QCOMPARE(spy.count(), 3);

    auto cmd= QString("!%1").arg(Helper::randomString());
    ctrl.setText(cmd);

    spy3.wait(10);
    QCOMPARE(spy3.count(), 1);
    QVERIFY(ctrl.diceCommand());

    ctrl.setText(Helper::randomString());
    cmd= QString("!%1").arg(Helper::randomString());
    ctrl.setText(cmd);

    spy3.wait(10);
    QCOMPARE(spy3.count(), 3);
    spy3.wait(1000);
    QCOMPARE(spy2.count(), 11);

    spy2.clear();

    static QStringList msgs{Helper::randomString(), Helper::randomString(), Helper::randomString(),
                            Helper::randomString()};

    ctrl.setText(msgs[0]);
    QCOMPARE(ctrl.text(), msgs[0]);
    ctrl.send();

    ctrl.setText(msgs[1]);
    QCOMPARE(ctrl.text(), msgs[1]);
    ctrl.send();

    ctrl.setText(msgs[2]);
    QCOMPARE(ctrl.text(), msgs[2]);
    ctrl.send();

    ctrl.setText(msgs[3]);
    QCOMPARE(ctrl.text(), msgs[3]);
    ctrl.send();

    ctrl.up();
    QCOMPARE(ctrl.text(), msgs[3]);
    ctrl.up();
    QCOMPARE(ctrl.text(), msgs[2]);

    ctrl.down();
    QCOMPARE(ctrl.text(), msgs[3]);

    QCOMPARE(spy2.count(), 11);

    for(int i= 0; i < 200; ++i)
    {
        ctrl.setText(Helper::randomString());
        ctrl.send();
    }

    for(int i= 0; i < 200; ++i)
        ctrl.up();

    for(int i= 0; i < 200; ++i)
        ctrl.down();
}

void InstantMessagingTest::diceMessageTest()
{
    auto owner= Helper::randomString();
    auto writer= Helper::randomString();

    InstantMessaging::DiceMessage msg(owner, writer, QDateTime::currentDateTime());

    msg.setText("{\n\"command\":\"8d10;\\\"Result: $1 [@1]\\\"\",\n   \"comment\":\"\",\n   \"error\":\"\",\n   "
                "\"instructions\":[\n      {\n         \"diceval\":[\n            {\n               \"color\":\"\",\n  "
                "             \"displayed\":false,\n               \"face\":10,\n               \"highlight\":true,\n  "
                "             \"string\":\"<span style=\\\"color:red;font-weight:bold;\\\">9</span>\",\n               "
                "\"uuid\":\"b8f031dc-04ec-41b6-9752-ae93f0b4d64f\",\n               \"value\":9\n            },\n      "
                "      {\n               \"color\":\"\",\n               \"displayed\":false,\n               "
                "\"face\":10,\n               \"highlight\":true,\n               \"string\":\"<span "
                "style=\\\"color:red;font-weight:bold;\\\">2</span>\",\n               "
                "\"uuid\":\"381cbf99-ba59-46d0-8350-596679315262\",\n               \"value\":2\n            },\n      "
                "      {\n               \"color\":\"\",\n               \"displayed\":false,\n               "
                "\"face\":10,\n               \"highlight\":true,\n               \"string\":\"<span "
                "style=\\\"color:red;font-weight:bold;\\\">4</span>\",\n               "
                "\"uuid\":\"c3632d2e-43af-41fb-97f3-d7e92d6d1f47\",\n               \"value\":4\n            },\n      "
                "      {\n               \"color\":\"\",\n               \"displayed\":false,\n               "
                "\"face\":10,\n               \"highlight\":true,\n               \"string\":\"<span "
                "style=\\\"color:red;font-weight:bold;\\\">9</span>\",\n               "
                "\"uuid\":\"f4962e7f-dd41-4fa9-afbe-f382a76274f0\",\n               \"value\":9\n            },\n      "
                "      {\n               \"color\":\"\",\n               \"displayed\":false,\n               "
                "\"face\":10,\n               \"highlight\":true,\n               \"string\":\"<span "
                "style=\\\"color:red;font-weight:bold;\\\">10</span>\",\n               "
                "\"uuid\":\"53679f15-6dde-418b-9743-a030df721c14\",\n               \"value\":10\n            },\n     "
                "       {\n               \"color\":\"\",\n               \"displayed\":false,\n               "
                "\"face\":10,\n               \"highlight\":true,\n               \"string\":\"<span "
                "style=\\\"color:red;font-weight:bold;\\\">6</span>\",\n               "
                "\"uuid\":\"20c8fe23-da7a-4319-9694-b93e6c4762d2\",\n               \"value\":6\n            },\n      "
                "      {\n               \"color\":\"\",\n               \"displayed\":false,\n               "
                "\"face\":10,\n               \"highlight\":true,\n               \"string\":\"<span "
                "style=\\\"color:red;font-weight:bold;\\\">6</span>\",\n               "
                "\"uuid\":\"28a8f62c-92b9-4784-bcb8-03d0880d6c14\",\n               \"value\":6\n            },\n      "
                "      {\n               \"color\":\"\",\n               \"displayed\":false,\n               "
                "\"face\":10,\n               \"highlight\":true,\n               \"string\":\"<span "
                "style=\\\"color:red;font-weight:bold;\\\">7</span>\",\n               "
                "\"uuid\":\"5aa9094b-e5f2-48b7-99bf-7d8a21ad7361\",\n               \"value\":7\n            }\n       "
                "  ],\n         \"scalar\":53\n      },\n      {\n         \"string\":\"Result: $1 [@1]\"\n      }\n   "
                "],\n   \"scalar\":\"53\",\n   \"string\":\"Result: 53 [<span "
                "style=\\\"color:red;font-weight:bold;\\\">9</span>,<span "
                "style=\\\"color:red;font-weight:bold;\\\">2</span>,<span "
                "style=\\\"color:red;font-weight:bold;\\\">4</span>,<span "
                "style=\\\"color:red;font-weight:bold;\\\">9</span>,<span "
                "style=\\\"color:red;font-weight:bold;\\\">10</span>,<span "
                "style=\\\"color:red;font-weight:bold;\\\">6</span>,<span "
                "style=\\\"color:red;font-weight:bold;\\\">6</span>,<span "
                "style=\\\"color:red;font-weight:bold;\\\">7</span>]\",\n   \"warning\":\"\"\n}");

    msg.setText("{\n\"command\":\"8d10;\\\"Result: $1 [@1]\\\"\",\n   \"comment\":\"\",\n   \"error\":\"\",\n   "
                "\"instructions\":[\n      {\n         \"diceval\":[\n            {\n               \"color\":\"\",\n  "
                "             \"displayed\":false,\n               \"face\":10,\n               \"highlight\":true,\n  "
                "             \"string\":\"<span style=\\\"color:red;font-weight:bold;\\\">9</span>\",\n               "
                "\"uuid\":\"b8f031dc-04ec-41b6-9752-ae93f0b4d64f\",\n               \"value\":9\n            },\n      "
                "      {\n               \"color\":\"\",\n               \"displayed\":false,\n               "
                "\"face\":10,\n               \"highlight\":true,\n               \"string\":\"<span "
                "style=\\\"color:red;font-weight:bold;\\\">2</span>\",\n               "
                "\"uuid\":\"381cbf99-ba59-46d0-8350-596679315262\",\n               \"value\":2\n            },\n      "
                "      {\n               \"color\":\"\",\n               \"displayed\":false,\n               "
                "\"face\":10,\n               \"highlight\":true,\n               \"string\":\"<span "
                "style=\\\"color:red;font-weight:bold;\\\">4</span>\",\n               "
                "\"uuid\":\"c3632d2e-43af-41fb-97f3-d7e92d6d1f47\",\n               \"value\":4\n            },\n      "
                "      {\n               \"color\":\"\",\n               \"displayed\":false,\n               "
                "\"face\":10,\n               \"highlight\":true,\n               \"string\":\"<span "
                "style=\\\"color:red;font-weight:bold;\\\">9</span>\",\n               "
                "\"uuid\":\"f4962e7f-dd41-4fa9-afbe-f382a76274f0\",\n               \"value\":9\n            },\n      "
                "      {\n               \"color\":\"\",\n               \"displayed\":false,\n               "
                "\"face\":10,\n               \"highlight\":true,\n               \"string\":\"<span "
                "style=\\\"color:red;font-weight:bold;\\\">10</span>\",\n               "
                "\"uuid\":\"53679f15-6dde-418b-9743-a030df721c14\",\n               \"value\":10\n            },\n     "
                "       {\n               \"color\":\"\",\n               \"displayed\":false,\n               "
                "\"face\":10,\n               \"highlight\":true,\n               \"string\":\"<span "
                "style=\\\"color:red;font-weight:bold;\\\">6</span>\",\n               "
                "\"uuid\":\"20c8fe23-da7a-4319-9694-b93e6c4762d2\",\n               \"value\":6\n            },\n      "
                "      {\n               \"color\":\"\",\n               \"displayed\":false,\n               "
                "\"face\":10,\n               \"highlight\":true,\n               \"string\":\"<span "
                "style=\\\"color:red;font-weight:bold;\\\">6</span>\",\n               "
                "\"uuid\":\"28a8f62c-92b9-4784-bcb8-03d0880d6c14\",\n               \"value\":6\n            },\n      "
                "      {\n               \"color\":\"\",\n               \"displayed\":false,\n               "
                "\"face\":10,\n               \"highlight\":true,\n               \"string\":\"<span "
                "style=\\\"color:red;font-weight:bold;\\\">7</span>\",\n               "
                "\"uuid\":\"5aa9094b-e5f2-48b7-99bf-7d8a21ad7361\",\n               \"value\":7\n            }\n       "
                "  ],\n         \"scalar\":53\n      },\n      {\n         \"string\":\"Result: $1 [@1]\"\n      }\n   "
                "],\n   \"scalar\":\"53\",\n   \"string\":\"Result: 53 [<span "
                "style=\\\"color:red;font-weight:bold;\\\">9</span>,<span "
                "style=\\\"color:red;font-weight:bold;\\\">2</span>,<span "
                "style=\\\"color:red;font-weight:bold;\\\">4</span>,<span "
                "style=\\\"color:red;font-weight:bold;\\\">9</span>,<span "
                "style=\\\"color:red;font-weight:bold;\\\">10</span>,<span "
                "style=\\\"color:red;font-weight:bold;\\\">6</span>,<span "
                "style=\\\"color:red;font-weight:bold;\\\">6</span>,<span "
                "style=\\\"color:red;font-weight:bold;\\\">7</span>]\",\n   \"warning\":\"\"\n}");

    msg.setText(
        "{\n    \"command\": \"2d6+2d8\",\n    \"comment\": \"\",\n    \"error\": \"\",\n    \"instructions\": [\n     "
        "   {\n            \"diceval\": [\n                {\n                    \"color\": \"\",\n                   "
        " \"displayed\": false,\n                    \"face\": 6,\n                    \"highlight\": true,\n          "
        "          \"string\": \"<span style=\\\"color:red;font-weight:bold;\\\">4</span>\",\n                    "
        "\"uuid\": \"f8077eac-d1d3-4711-bc1e-1f2ff6532d0c\",\n                    \"value\": 4\n                },\n   "
        "             {\n                    \"color\": \"\",\n                    \"displayed\": false,\n             "
        "       \"face\": 6,\n                    \"highlight\": true,\n                    \"string\": \"<span "
        "style=\\\"color:red;font-weight:bold;\\\">5</span>\",\n                    \"uuid\": "
        "\"763015b9-3764-43d9-8ef9-ae55cde23094\",\n                    \"value\": 5\n                },\n             "
        "   {\n                    \"color\": \"\",\n                    \"displayed\": false,\n                    "
        "\"face\": 8,\n                    \"highlight\": true,\n                    \"string\": \"<span "
        "style=\\\"color:red;font-weight:bold;\\\">3</span>\",\n                    \"uuid\": "
        "\"ecacc278-f5b8-41bd-a936-df29101c6d25\",\n                    \"value\": 3\n                },\n             "
        "   {\n                    \"color\": \"\",\n                    \"displayed\": false,\n                    "
        "\"face\": 8,\n                    \"highlight\": true,\n                    \"string\": \"<span "
        "style=\\\"color:red;font-weight:bold;\\\">4</span>\",\n                    \"uuid\": "
        "\"ae62c050-9155-4e50-8b01-ee9a8f3e0906\",\n                    \"value\": 4\n                }\n            "
        "],\n            \"scalar\": 16\n        }\n    ],\n    \"scalar\": \"16\",\n    \"string\": \"16\",\n    "
        "\"warning\": \"\"\n}\n");

    QCOMPARE(msg.command(), "2d6+2d8");
    QCOMPARE(msg.comment(), "");
    QVERIFY(!msg.result().isEmpty());
    QVERIFY(!msg.details().isEmpty());
    QVERIFY(!msg.text().isEmpty());
}
QTEST_MAIN(InstantMessagingTest);

#include "tst_im.moc"
