/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
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

#include <QSignalSpy>
#include <QTest>

#include "charactersheet/charactersheet.h"
#include "controller/view_controller/charactersheetcontroller.h"
#include "data/character.h"
#include "data/player.h"
#include "model/charactermodel.h"
#include "model/playermodel.h"
#include <QAbstractItemModelTester>
#include <helper.h>
#include <memory>

class CharacterSheetControllerTest : public QObject
{
    Q_OBJECT
public:
    CharacterSheetControllerTest()= default;

private slots:
    void init();
    void cleanupTestCase();

    void gameMasterTest();

    void sharingDataTest();

private:
    std::unique_ptr<CharacterSheetController> m_ctrl;
    std::unique_ptr<CharacterModel> m_characterModel;
    std::unique_ptr<PlayerModel> m_playerModel;
    std::vector<std::unique_ptr<QAbstractItemModelTester>> m_testers;
};

void CharacterSheetControllerTest::init()
{
    m_playerModel.reset(new PlayerModel());
    m_characterModel.reset(new CharacterModel());
    m_testers.push_back(std::make_unique<QAbstractItemModelTester>(m_playerModel.get()));
    m_characterModel->setSourceModel(m_playerModel.get());
    m_testers.push_back(std::make_unique<QAbstractItemModelTester>(m_characterModel.get()));
    m_ctrl.reset(new CharacterSheetController());

    CharacterSheetController::setCharacterModel(m_characterModel.get());
}

void CharacterSheetControllerTest::cleanupTestCase() {}

void CharacterSheetControllerTest::gameMasterTest()
{
    QCOMPARE(m_ctrl->gameMasterId(), QString());
    m_testers.push_back(std::make_unique<QAbstractItemModelTester>(m_ctrl->model()));

    QSignalSpy spy(m_ctrl.get(), &CharacterSheetController::gameMasterIdChanged);

    m_ctrl->setGameMasterId("GameMasterID");
    m_ctrl->setGameMasterId("GameMasterID");

    QCOMPARE(m_ctrl->gameMasterId(), QString("GameMasterID"));
    QCOMPARE(spy.count(), 1);

    auto qml= Helper::randomString();
    m_ctrl->setQmlCode(qml);

    QVERIFY(!m_ctrl->cornerEnabled());

    QJsonObject obj;
    m_ctrl->setRootJson(obj);
    m_ctrl->setRootJson(obj);

    auto idsheet= Helper::randomString();
    auto id= Helper::randomString();

    obj["name"]= Helper::randomString();
    obj["idSheet"]= idsheet;
    QJsonObject array= QJsonObject();

    for(int i= 0; i < 10; ++i)
    {
        QJsonObject item;
        item["type"]= "field";
        item["typefield"]= Helper::generate<int>(0, 15);
        item["id"]= QString::number(i);
        item["label"]= Helper::randomString();
        item["value"]= Helper::randomString();
        item["formula"]= QString{};
        item["readonly"]= false;
        array[Helper::randomString()]= item;
    }
    obj["values"]= array;

    QSignalSpy spy2(m_ctrl.get(), &CharacterSheetController::sheetCreated);
    m_ctrl->addCharacterSheet(obj, id);

    Player* p1= new Player;
    p1->setUuid(id);

    spy2.wait(10);
    QCOMPARE(spy2.count(), 0);

    m_playerModel->addPlayer(p1);
    m_ctrl->addCharacterSheet(obj, id);

    spy2.wait(10);
    QCOMPARE(spy2.count(), 0);

    p1->setUuid(Helper::randomString());

    Character* charac= new Character;
    charac->setNpc(false);
    charac->setUuid(id);
    p1->addCharacter(charac);

    m_ctrl->addCharacterSheet(obj, id);

    spy2.wait(10);
    QCOMPARE(spy2.count(), 1);

    QSignalSpy spyAll(m_ctrl.get(), &CharacterSheetController::share);

    m_ctrl->setLocalGM(false);
    m_ctrl->shareCharacterSheetToAll(0);
    spyAll.wait(10);
    QCOMPARE(spyAll.count(), 0);

    m_ctrl->setLocalGM(true);
    m_ctrl->shareCharacterSheetToAll(80);
    spyAll.wait(10);
    QCOMPARE(spyAll.count(), 0);

    m_ctrl->shareCharacterSheetToAll(0);
    spyAll.wait(10);
    QCOMPARE(spyAll.count(), 1);

    m_ctrl->stopSharing(idsheet);

    spyAll.clear();

    m_ctrl->shareCharacterSheetTo(Helper::randomString(), 0);
    spyAll.wait(10);
    QCOMPARE(spyAll.count(), 0);
    m_ctrl->shareCharacterSheetTo(p1->uuid(), 80);
    spyAll.wait(10);
    QCOMPARE(spyAll.count(), 0);

    m_ctrl->shareCharacterSheetTo(p1->uuid(), 0);
    spyAll.wait(10);
    QCOMPARE(spyAll.count(), 0);

    m_ctrl->shareCharacterSheetTo(charac->uuid(), 0);
    spyAll.wait(10);
    QCOMPARE(spyAll.count(), 1);
}

void CharacterSheetControllerTest::sharingDataTest()
{
    // Create sheets
    auto sheets= m_ctrl->model();
    m_ctrl->setLocalGM(true);
    CharacterSheet* sheet1= new CharacterSheet;
    sheet1->setName("sheet1");
    sheet1->setUuid("sheet1");
    CharacterSheet* sheet2= new CharacterSheet;
    sheet2->setName("sheet2");
    sheet2->setUuid("sheet2");
    sheets->addCharacterSheet(sheet1);
    sheets->addCharacterSheet(sheet2);

    // set characters
    auto gm= new Player();
    gm->setGM(true);
    gm->setUuid("GM");
    gm->setName("GM");
    m_ctrl->setLocalId("GM");
    m_playerModel->addPlayer(gm);

    auto player1= new Player();
    player1->setGM(false);
    player1->setUuid("player1");
    player1->setName("player1");

    auto charact1= new Character();
    charact1->setName("Charact1");
    charact1->setUuid("Charact1");
    player1->addCharacter(charact1);

    m_playerModel->addPlayer(player1);

    auto player2= new Player();
    player2->setGM(false);
    player2->setUuid("player2");
    player2->setName("player2");

    auto charact2= new Character();
    charact2->setName("charact2");
    charact2->setUuid("charact2");
    player2->addCharacter(charact2);

    m_playerModel->addPlayer(player2);

    QSignalSpy spy(m_ctrl.get(), &CharacterSheetController::sheetCreated);
    QSignalSpy spy1(m_ctrl.get(), &CharacterSheetController::share);

    // share sheet
    m_ctrl->shareCharacterSheetTo(charact1, sheet1);

    spy.wait();
    spy1.wait();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy1.count(), 1);

    spy.clear();
    spy1.clear();

    // reaffect same sheet to same character
    m_ctrl->shareCharacterSheetTo(charact1, sheet1);
    spy.wait();
    spy1.wait();
    QCOMPARE(spy.count(), 0);
    QCOMPARE(spy1.count(), 0);

    auto data= m_ctrl->sheetData();
    QCOMPARE(data.count(), 1);

    spy.clear();
    spy1.clear();
    // simulate error of sharing 2 sheet to the same character.
    m_ctrl->shareCharacterSheetTo(charact1, sheet2);
    spy.wait();
    spy1.wait();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy1.count(), 1);

    data= m_ctrl->sheetData();
    QCOMPARE(data.count(), 2);
    spy.clear();
    spy1.clear();

    // simulate fixing error
    m_ctrl->shareCharacterSheetTo(charact2, sheet2);
    spy.wait();
    spy1.wait();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy1.count(), 1);

    data= m_ctrl->sheetData();
    for(auto const& share : std::as_const(data))
    {
        qDebug() << share.m_characterId << share.m_sheetId;
        // QCOMPARE(share.m_characterId, charact1->uuid());
    }
    QCOMPARE(data.count(), 2);
}
QTEST_MAIN(CharacterSheetControllerTest);

#include "tst_charactersheetcontrollertest.moc"
