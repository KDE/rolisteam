#include "data/character.h"
#include "data/player.h"
#include "helper.h"
#include "model/charactermodel.h"
#include "model/nonplayablecharactermodel.h"
#include "model/participantmodel.h"
#include "model/playermodel.h"
#include "worker/characterfinder.h"
#include <QAbstractItemModelTester>
#include <QTest>
#include <memory>

class PlayerModelTest : public QObject
{
    Q_OBJECT
public:
    PlayerModelTest();

private slots:
    void init();
    void addTest();
    void addTest_data();

    void ownerTest();
    void ownerTest_data();

    void finderTest();
    void modelTest();
    void writeModelTest();

private:
    std::unique_ptr<PlayerModel> m_playerModel;
    std::unique_ptr<ParticipantModel> m_participantsModel;
    std::vector<std::unique_ptr<QAbstractItemModelTester>> m_tester;
};

void PlayerModelTest::init()
{
    m_playerModel.reset(new PlayerModel());
    m_participantsModel.reset(new ParticipantModel("", m_playerModel.get()));

    m_tester.push_back(std::make_unique<QAbstractItemModelTester>(m_playerModel.get()));
    m_tester.push_back(std::make_unique<QAbstractItemModelTester>(m_participantsModel.get()));

    // m_participantsModel->setSourceModel(m_playerModel.get());
}

PlayerModelTest::PlayerModelTest()= default;

void PlayerModelTest::addTest()
{
    QFETCH(int, count);
    QFETCH(QString, ownerId);
    QFETCH(int, idx);
    QFETCH(QString, name);
    QFETCH(QVector<Player*>, players);

    m_participantsModel->setOwner(ownerId);

    m_playerModel->clear();
    QVERIFY(m_playerModel->rowCount() == 0);

    auto index= m_participantsModel->index(idx, 0, QModelIndex());

    QCOMPARE(m_participantsModel->rowCount(index), 0);

    for(auto p : players)
        m_playerModel->addPlayer(p);

    QCOMPARE(m_participantsModel->rowCount(index), count);
    if(count > 0)
    {
        auto data= m_participantsModel->index(0, 0, index);
        QCOMPARE(name, data.data().toString());
    }

    /*QJsonObject obj;

    m_participantsModel->saveModel(obj);
    m_participantsModel->loadModel(obj);*/

    for(auto p : players)
        m_participantsModel->removePlayer(p);
}

void PlayerModelTest::addTest_data()
{
    QTest::addColumn<int>("count");
    QTest::addColumn<QString>("ownerId");
    QTest::addColumn<int>("idx");
    QTest::addColumn<QString>("name");
    QTest::addColumn<QVector<Player*>>("players");

    QTest::addRow("cmd 1") << 0 << QString() << 0 << QString() << QVector<Player*>();

    {
        QVector<Player*> players;
        players.push_back(new Player("name", QColor(Qt::red), true));
        QTest::addRow("cmd 2") << 1 << players[0]->uuid() << 0 << players[0]->name() << players;
    }

    {
        QVector<Player*> players;
        auto p= new Player("name", QColor(Qt::red), true);
        players.push_back(p);
        QTest::addRow("cmd 3") << 1 << p->uuid() << 0 << p->name() << players;
    }

    {
        QVector<Player*> players;
        auto p= new Player("name", QColor(Qt::red), true);
        players.push_back(p);
        QTest::addRow("cmd 4") << 1 << "tata" << 2 << p->name() << players;
    }

    {
        QVector<Player*> players;
        auto owner= new Player("owner", QColor(Qt::red), true);
        players.push_back(owner);
        auto p= new Player("contributor", QColor(Qt::green), false);
        players.push_back(p);
        QTest::addRow("cmd 5") << 1 << owner->uuid() << 2 << p->name() << players;
    }

    /*{
        QVector<Player*> players;
        auto player= new Player("name", QColor(Qt::red), true);
        player->addCharacter("character", QColor(Qt::blue), "", QHash<QString, QVariant>(), false);
        players.push_back(player);
        QTest::addRow("cmd 3") << 1 << players;
    }

    {
        QVector<Player*> players;
        auto player= new Player("name", QColor(Qt::red), true);
        player->addCharacter(new Character("character", QColor(Qt::blue), true));
        players.push_back(player);
        QTest::addRow("cmd 4") << 1 << players;
    }

    {
        QVector<Player*> players;
        auto player= new Player("name", QColor(Qt::red), true);
        player->addCharacter(new Character("character", QColor(Qt::blue), false));
        player->addCharacter(new Character("character2", QColor(Qt::green), false));
        players.push_back(player);
        QTest::addRow("cmd 5") << 2 << players;
    }

    {
        QVector<Player*> players;
        auto player= new Player("name", QColor(Qt::red), true);
        player->addCharacter(new Character("character", QColor(Qt::blue), false));
        players.push_back(player);

        auto player2= new Player("name2", QColor(Qt::magenta), true);
        player2->addCharacter(new Character("character2", QColor(Qt::green), false));
        players.push_back(player2);

        QTest::addRow("cmd 6") << 2 << players;
    }

    {
        QVector<Player*> players;
        auto player= new Player("name", QColor(Qt::red), true);
        players.push_back(player);

        auto player2= new Player("name2", QColor(Qt::magenta), true);
        player2->addCharacter(new Character("character", QColor(Qt::blue), false));
        player2->addCharacter(new Character("character2", QColor(Qt::green), false));
        players.push_back(player2);
        QTest::addRow("cmd 7") << 2 << players;
    }*/
}

void PlayerModelTest::ownerTest()
{
    QFETCH(int, count);
    QFETCH(QString, ownerId);
    QFETCH(int, idx);
    QFETCH(QVector<Player*>, players);

    m_playerModel->clear();
    QVERIFY(m_playerModel->rowCount() == 0);

    auto index= m_participantsModel->index(idx, 0, QModelIndex());

    QCOMPARE(m_participantsModel->rowCount(index), 0);

    for(auto p : players)
    {
        m_playerModel->addPlayer(p);
    }
    m_participantsModel.reset(new ParticipantModel("", m_playerModel.get()));

    m_participantsModel->setOwner(ownerId);

    index= m_participantsModel->index(idx, 0, QModelIndex());

    QCOMPARE(m_participantsModel->rowCount(index), count);
}

void PlayerModelTest::ownerTest_data()
{
    QTest::addColumn<int>("count");
    QTest::addColumn<QString>("ownerId");
    QTest::addColumn<int>("idx");
    QTest::addColumn<QVector<Player*>>("players");

    QTest::addRow("cmd 1") << 0 << QString() << 0 << QVector<Player*>();

    {
        QVector<Player*> players;
        auto p= new Player("name", QColor(Qt::red), true);
        players.push_back(p);
        QTest::addRow("cmd 2") << 1 << p->uuid() << 0 << players;
    }

    {
        QVector<Player*> players;
        auto p= new Player("name", QColor(Qt::red), true);
        players.push_back(p);
        QTest::addRow("cmd 3") << 1 << "tata" << 2 << players;
    }

    {
        QVector<Player*> players;
        auto owner= new Player("owner", QColor(Qt::red), true);
        players.push_back(owner);
        auto p= new Player("contributor", QColor(Qt::green), false);
        players.push_back(p);
        QTest::addRow("cmd 4") << 1 << owner->uuid() << 2 << players;
    }
}

void PlayerModelTest::finderTest()
{
    auto owner= new Player(Helper::randomString(), QColor(Qt::red), true);
    auto cha= new Character(Helper::randomString(), Helper::randomColor(), false);
    owner->addCharacter(cha);
    m_playerModel->addPlayer(owner);
    auto id= cha->uuid();
    CharacterFinder finder;

    QVERIFY(!finder.find(id));

    CharacterFinder::setPlayerModel(m_playerModel.get());
    finder.setUpConnect();
    QVERIFY(!finder.find(id));
    CharacterFinder::setNpcModel(new campaign::NonPlayableCharacterModel(nullptr));
    QVERIFY(!finder.find(id));
    auto pm= new CharacterModel();
    pm->setSourceModel(m_playerModel.get());
    CharacterFinder::setPcModel(pm);

    {
        auto owner= new Player(Helper::randomString(), QColor(Qt::red), true);
        m_playerModel->addPlayer(owner);
    }
    QVERIFY(finder.find(id));
}

void PlayerModelTest::modelTest()
{
    m_playerModel->clear(true);
    auto p= new Player(Helper::randomString(), QColor(Qt::red), true);
    m_playerModel->addPlayer(nullptr);
    m_playerModel->addPlayer(p);
    m_playerModel->addPlayer(p);
    auto c= new Character(Helper::randomString(), Helper::randomColor(), false);
    m_playerModel->addCharacter(m_playerModel->index(0, 0), c);

    QCOMPARE(p->characterCount(), 1);

    m_playerModel->removeCharacter(c);

    QCOMPARE(p->characterCount(), 0);

    auto roles= m_playerModel->roleNames();
    QCOMPARE(roles.size(), 9);
}

void PlayerModelTest::writeModelTest()
{
    m_playerModel->clear(true);
    auto p= new Player(Helper::randomString(), QColor(Qt::red), true);
    m_playerModel->addPlayer(p);

    auto id= m_playerModel->personToIndex(p);

    auto idx= m_playerModel->index(0, 0);
    QCOMPARE(idx, id);

    auto v= Helper::randomString();
    m_playerModel->setData(idx, v, Qt::DisplayRole);
    QCOMPARE(m_playerModel->data(idx, Qt::DisplayRole).toString(), v);

    v= Helper::randomString();
    QVERIFY(m_playerModel->setData(idx, v, Qt::EditRole));
    QCOMPARE(m_playerModel->data(idx, Qt::EditRole).toString(), v);

    v= Helper::randomString();
    QVERIFY(m_playerModel->setData(idx, v, PlayerModel::NameRole));
    QCOMPARE(m_playerModel->data(idx, PlayerModel::NameRole).toString(), v);

    QVERIFY(!m_playerModel->setData(idx, false, PlayerModel::GmRole));
    QCOMPARE(m_playerModel->data(idx, PlayerModel::GmRole).toBool(), true);

    v= Helper::randomString();
    QVERIFY(!m_playerModel->setData(idx, v, PlayerModel::IdentifierRole));
    QVERIFY(m_playerModel->data(idx, PlayerModel::IdentifierRole).toString() != v);

    auto c= Helper::randomColor();
    m_playerModel->setData(idx, c, PlayerModel::ColorRole);
    QCOMPARE(m_playerModel->data(idx, PlayerModel::ColorRole).value<QColor>(), c);

    QVERIFY(!m_playerModel->setData(idx, true, PlayerModel::LocalRole));
    QVERIFY(!m_playerModel->setData(idx, true, PlayerModel::CharacterRole));

    QVERIFY(!m_playerModel->setData(idx, Helper::randomString(), PlayerModel::CharacterStateIdRole));
    QVERIFY(!m_playerModel->setData(idx, true, PlayerModel::NpcRole));
    QVERIFY(!m_playerModel->setData(idx, Helper::imageData(true), PlayerModel::AvatarRole));
}

QTEST_MAIN(PlayerModelTest);

#include "tst_playermodel.moc"
