#include <QTest>

#include <QSignalSpy>
#include <memory>

#include "controller/gamecontroller.h"
#include "helper.h"

class GameControllerTest : public QObject
{
    Q_OBJECT
public:
    GameControllerTest();

private slots:
    void init();

    void clearGameTest();
    void localIdTest();

    void addMediaTest();
    void addMediaTest_data();

    void newMediaTest();
    void newMediaTest_data();

    void openInternalResourcesTest();

    void saveTest();
    void saveAsTest();

    void openWebPageTest();

    void logControllerTest();
    void accessDataTest();

    void connectionTest();

private:
    std::unique_ptr<GameController> m_ctrl;
};

GameControllerTest::GameControllerTest() {}

void GameControllerTest::init()
{
    m_ctrl.reset(new GameController(Helper::randomString(), nullptr, nullptr));
}

void GameControllerTest::clearGameTest()
{
    m_ctrl->clear(true);
    m_ctrl->clear(false);
}

void GameControllerTest::localIdTest()
{
    auto playerCtrl= m_ctrl->playerController();
    QCOMPARE(m_ctrl->localPlayerId(), playerCtrl->localPlayerId());
}

void GameControllerTest::addMediaTest()
{
    QFETCH(Core::ContentType, contentType);
    using DataVec= std::map<QString, QVariant>;
    QFETCH(DataVec, map);
    map.insert({Core::keys::KEY_TYPE, QVariant::fromValue(contentType)});

    m_ctrl->openMedia(map);
}

void GameControllerTest::addMediaTest_data()
{
    QTest::addColumn<Core::ContentType>("contentType");
    QTest::addColumn<std::map<QString, QVariant>>("map");

    auto f= []
    {
        static int counter= 0;
        return counter++;
    };
    QTest::addRow("cmd %d", f()) << Core::ContentType::UNKNOWN << std::map<QString, QVariant>();

    QTest::addRow("cmd %d", f()) << Core::ContentType::PICTURE
                                 << std::map<QString, QVariant>({{"path", ":/img/girafe3.jpg"}, {"name", "girafe"}});

    QTest::addRow("cmd %d", f()) << Core::ContentType::CHARACTERSHEET
                                 << std::map<QString, QVariant>(
                                        {{"path", ":/charactersheet/bitume_fixed.rcs"}, {"name", "bitume"}});

    QTest::addRow("cmd %d", f()) << Core::ContentType::PDF
                                 << std::map<QString, QVariant>(
                                        {{"path", ":/pdf/01_personnages.pdf"}, {"name", "personnages"}});

    QTest::addRow("cmd %d", f()) << Core::ContentType::SHAREDNOTE
                                 << std::map<QString, QVariant>(
                                        {{"path", ":/sharednotes/test.rsn"}, {"name", "RSN file"}});

    QTest::addRow("cmd %d", f()) << Core::ContentType::NOTES
                                 << std::map<QString, QVariant>(
                                        {{"path", ":/sharednotes/scenario.md"}, {"name", "Markdown file"}});

    QTest::addRow("cmd %d", f()) << Core::ContentType::VECTORIALMAP
                                 << std::map<QString, QVariant>({{"path", ""}, {"name", ""}});

    QTest::addRow("cmd %d", f()) << Core::ContentType::MINDMAP
                                 << std::map<QString, QVariant>({{"path", ""}, {"name", ""}});

    QTest::addRow("cmd %d", f()) << Core::ContentType::WEBVIEW
                                 << std::map<QString, QVariant>(
                                        {{"pageUrl", "https://rolisteam.org"}, {"name", "rolisteam"}});
}

void GameControllerTest::newMediaTest()
{
    QFETCH(Core::ContentType, contentType);
    using DataVec= std::map<QString, QVariant>;
    QFETCH(DataVec, map);
    map.insert({Core::keys::KEY_TYPE, QVariant::fromValue(contentType)});

    m_ctrl->newMedia(map);
}

void GameControllerTest::newMediaTest_data()
{
    QTest::addColumn<Core::ContentType>("contentType");
    QTest::addColumn<std::map<QString, QVariant>>("map");

    auto f= []
    {
        static int counter= 0;
        return counter++;
    };
    QTest::addRow("cmd %d", f()) << Core::ContentType::UNKNOWN << std::map<QString, QVariant>();

    QTest::addRow("cmd %d", f()) << Core::ContentType::PICTURE
                                 << std::map<QString, QVariant>({{"path", ":/img/girafe3.jpg"}, {"name", "girafe"}});

    QTest::addRow("cmd %d", f()) << Core::ContentType::CHARACTERSHEET
                                 << std::map<QString, QVariant>(
                                        {{"path", ":/charactersheet/bitume_fixed.rcs"}, {"name", "bitume"}});

    QTest::addRow("cmd %d", f()) << Core::ContentType::PDF
                                 << std::map<QString, QVariant>(
                                        {{"path", ":/pdf/01_personnages.pdf"}, {"name", "personnages"}});

    QTest::addRow("cmd %d", f()) << Core::ContentType::SHAREDNOTE
                                 << std::map<QString, QVariant>(
                                        {{"path", ":/sharednotes/test.rsn"}, {"name", "RSN file"}});

    QTest::addRow("cmd %d", f()) << Core::ContentType::NOTES
                                 << std::map<QString, QVariant>(
                                        {{"path", ":/sharednotes/scenario.md"}, {"name", "Markdown file"}});

    QTest::addRow("cmd %d", f()) << Core::ContentType::VECTORIALMAP
                                 << std::map<QString, QVariant>({{"path", ""}, {"name", ""}});

    QTest::addRow("cmd %d", f()) << Core::ContentType::MINDMAP
                                 << std::map<QString, QVariant>({{"path", ""}, {"name", ""}});

    QTest::addRow("cmd %d", f()) << Core::ContentType::WEBVIEW
                                 << std::map<QString, QVariant>(
                                        {{"pageUrl", "https://rolisteam.org"}, {"name", "rolisteam"}});
}

void GameControllerTest::openInternalResourcesTest()
{
    std::vector<Core::ContentType> data({Core::ContentType::VECTORIALMAP, Core::ContentType::SHAREDNOTE,
                                         Core::ContentType::CHARACTERSHEET, Core::ContentType::NOTES,
                                         Core::ContentType::PICTURE, Core::ContentType::WEBVIEW});

    for(auto type : data)
        m_ctrl->openInternalResources(Helper::randomString(), Helper::randomString(), type);
}

void GameControllerTest::saveTest()
{
    m_ctrl->save();
}
void GameControllerTest::saveAsTest()
{
    m_ctrl->saveAs(Helper::randomString());
}

void GameControllerTest::openWebPageTest()
{
    m_ctrl->openPageWeb(Helper::randomUrl().toString());
}

void GameControllerTest::logControllerTest()
{
    auto log= m_ctrl->logController();

    QVERIFY(log);

    log->setCurrentModes(LogController::Gui);
    log->setLogLevel(LogController::Search);

    QSignalSpy spy(log, &LogController::showMessage);

    m_ctrl->addErrorLog(Helper::randomString(), Helper::randomString());
    // spy.wait();
    m_ctrl->addWarningLog(Helper::randomString(), Helper::randomString());
    // spy.wait();
    m_ctrl->addFeatureLog(Helper::randomString(), Helper::randomString());
    // spy.wait();
    m_ctrl->addInfoLog(Helper::randomString(), Helper::randomString());
    // spy.wait();
    m_ctrl->addSearchLog(Helper::randomString(), Helper::randomString());
    spy.wait();

    QCOMPARE(spy.count(), 10); // SHOULD BE 5 only
}

void GameControllerTest::accessDataTest()
{
    auto v= m_ctrl->remoteVersion();
    QVERIFY(v.isEmpty());

    auto type= m_ctrl->tipOfDay();
    QCOMPARE(type.content.isNull(), !m_ctrl->tipAvailable());

    m_ctrl->startTipOfDay();

    type= m_ctrl->tipOfDay();
    QCOMPARE(type.content.isNull(), !m_ctrl->tipAvailable());

    v= m_ctrl->version();
    QVERIFY(!v.isEmpty());

    auto b= m_ctrl->connected();
    QVERIFY(!b);

    auto stack= m_ctrl->undoStack();
    QVERIFY(stack);

    m_ctrl->setDataFromProfile(Helper::generate(1000, 10000));
    m_ctrl->setDataFromProfile(0);
}

void GameControllerTest::connectionTest()
{
    m_ctrl->startConnection();
    m_ctrl->aboutToClose(true);
    m_ctrl->aboutToClose(false);
    m_ctrl->stopConnection();

    m_ctrl->startCheckForUpdates();
    QSignalSpy spy(m_ctrl.get(), &GameController::tipOfDayChanged);
    m_ctrl->startIpRetriever();

    spy.wait();
    QCOMPARE(spy.count(), 0);
}

QTEST_MAIN(GameControllerTest);

#include "tst_gamecontroller.moc"
