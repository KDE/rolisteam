#include <QAbstractItemModelTester>
#include <QObject>
#include <QTest>
#include <QtCore/QCoreApplication>

#include "helper.h"

#include "charactersheet/include/charactersheet/controllers/tablefield.h"
#include "controller/view_controller/charactersheetcontroller.h"
#include "data/campaignmanager.h"
#include "data/player.h"
#include "diceparser_qobject/diceroller.h"
#include "model/contentmodel.h"
#include "model/playermodel.h"
#include "rcse/fieldmodel.h"
#include "updater/media/charactersheetupdater.h"

class SheetTest : public QObject
{
    Q_OBJECT
public:
    SheetTest();

private slots:
    void init();

    void tableTest();
    void tableTest_data();

    void updateTest();

private:
    std::unique_ptr<FieldModel> m_model;
    std::unique_ptr<QAbstractItemModelTester> m_tester;
    std::unique_ptr<campaign::CampaignManager> m_campaign;
    std::unique_ptr<CharacterSheet> m_sheet;
    std::unique_ptr<CharacterSheetController> m_ctrl;
    std::unique_ptr<CharacterSheetUpdater> m_updater;
    std::unique_ptr<PlayerModel> m_players;
    std::unique_ptr<FilteredContentModel> m_sheetModel;
    std::unique_ptr<ContentModel> m_contentModel;
};

SheetTest::SheetTest() {}

void SheetTest::init()
{
    m_model.reset(new FieldModel);
    m_tester.reset(new QAbstractItemModelTester(m_model.get()));
    m_players.reset(new PlayerModel);
    m_contentModel.reset(new ContentModel);
    m_sheetModel.reset(new FilteredContentModel(Core::ContentType::CHARACTERSHEET));
    m_sheetModel->setSourceModel(m_contentModel.get());
}

void SheetTest::tableTest()
{
    QFETCH(int, lineCount);
    QFETCH(int, colCount);
    QFETCH(int, expectedData);

    TableFieldController* field= new TableFieldController();
    m_model->appendField(field);

    for(auto i= 0; i < colCount; ++i)
        field->addColumn();

    for(auto i= 0; i < lineCount; ++i)
        field->addLine();

    QCOMPARE(m_model->rowCount(QModelIndex()), 1);
    QCOMPARE(m_model->rowCount(m_model->index(0, 0, QModelIndex())), expectedData);

    auto parent= m_model->index(0, 0, QModelIndex());
    auto count= m_model->rowCount(m_model->index(0, 0, QModelIndex()));
    for(int i= colCount + 1; i < count; ++i)
    {
        auto id= m_model->data(m_model->index(i, 0, parent), Qt::DisplayRole).toString();
        QString val= QString("cell_%1").arg(i - colCount);
        QCOMPARE(id, val);
    }
}

void SheetTest::tableTest_data()
{
    QTest::addColumn<int>("lineCount");
    QTest::addColumn<int>("colCount");
    QTest::addColumn<int>("expectedData");

    QTest::addRow("empty") << 0 << 0 << 2;
    QTest::addRow("cmd1") << 1 << 0 << 3;
    QTest::addRow("cmd2") << 3 << 1 << 10;
}

void SheetTest::updateTest()
{
    m_sheet.reset(new CharacterSheet());
    m_ctrl.reset(new CharacterSheetController("mytestid"));
    CharacterFinder::setPlayerModel(m_players.get());
    auto player= new Player();
    player->setName(Helper::randomString());
    player->setGM(true);
    m_players->addPlayer(player);

    m_campaign.reset(new campaign::CampaignManager{new DiceRoller});

    m_updater.reset(new CharacterSheetUpdater(m_sheetModel.get(), m_campaign.get()));
    m_updater->addMediaController(nullptr);

    m_updater->setLocalIsGM(true);

    auto field= new FieldController();
    m_sheet->insertCharacterItem(field);

    auto table= new TableFieldController();
    table->addColumn();
    table->addLine();
    m_sheet->insertCharacterItem(table);

    Helper::TestMessageSender sender;
    NetworkMessage::setMessageSender(&sender);
    sender.clear();
    {
        auto model= m_ctrl->model();
        model->addCharacterSheet(m_sheet.get());

        m_updater->addMediaController(m_ctrl.get());
    }

    m_ctrl->share(m_ctrl.get(), m_sheet.get(), CharacterSheetUpdater::SharingMode::ALL, nullptr, QStringList{});
    m_ctrl->setModified(true);
    m_ctrl->setModified(false);
    m_ctrl->setModified(true);

    m_ctrl->removedSheet(m_sheet->uuid(), m_ctrl->uuid(), QString{});

    m_updater->setUpFieldUpdate(m_sheet.get());
    m_sheet->updateField(m_sheet.get(), field, field->id());
    m_sheet->updateTableFieldCellValue(m_sheet.get(), table->id(), 0, 0);
    m_sheet->tableRowCountChanged(true, m_sheet.get(), table, table->id(), 0);

    m_ctrl->setRemote(false);
    m_updater->addRemoteCharacterSheet(m_ctrl.get());
    m_ctrl->setRemote(true);
    m_updater->addRemoteCharacterSheet(m_ctrl.get());

    auto datas= sender.messageData();
    for(auto const& msgData : datas)
    {
        NetworkMessageReader reader;
        reader.setData(msgData);
        m_updater->processMessage(&reader);
    }

    m_sheet.release(); // data destroyed by model.
}

QTEST_MAIN(SheetTest);

#include "tst_sheet.moc"
