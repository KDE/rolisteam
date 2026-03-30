/***************************************************************************
 *   Copyright (C) 2018 by Renaud Guezennec                                *
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
#include <QAbstractItemModelTester>
#include <QMouseEvent>
#include <QSignalSpy>
#include <QTest>
#include <map>

#include "controller/item_controllers/imageitemcontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"

#include "controller/view_controller/charactersheetcontroller.h"
#include "controller/view_controller/imagecontroller.h"
#include "controller/view_controller/mindmapcontroller.h"
#include "controller/view_controller/notecontroller.h"
#include "controller/view_controller/pdfcontroller.h"
#include "controller/view_controller/sharednotecontroller.h"
#include "controller/view_controller/webpagecontroller.h"

#include "data/campaignmanager.h"
#include "media/mediafactory.h"
#include "worker/iohelper.h"
#include "worker/vectorialmapmessagehelper.h"
#include <controller/contentcontroller.h>
#include <controller/playercontroller.h>
#include <data/character.h>
#include <helper.h>
#include <model/contentmodel.h>

#include "media/mediatype.h"

class ContentControllerTest : public QObject
{
    Q_OBJECT

public:
    ContentControllerTest();

private slots:
    void init();
    void serializeTest();
    void serializeTest_data();

    void saveLoadImage();
    void testFilteredData();

    void themeTest();
    void modelTest();
    void modelTest_data();

    void controllerTest();

    void playerControllerTest();

private:
    std::unique_ptr<ContentController> m_ctrl;
    std::unique_ptr<PlayerController> m_playerCtrl;
    std::unique_ptr<ContentModel> m_model;
    std::unique_ptr<FilteredContentModel> m_filteredModel;
    std::vector<std::unique_ptr<QAbstractItemModelTester>> m_tester;
    QUndoStack m_stack;
};

Q_DECLARE_METATYPE(std::vector<Core::ContentType>)

ContentControllerTest::ContentControllerTest() {}

void ContentControllerTest::init()
{
    m_playerCtrl.reset(new PlayerController(nullptr));
    m_ctrl.reset(new ContentController(nullptr, m_playerCtrl->model(), m_playerCtrl->characterModel(), nullptr));
    connect(m_ctrl.get(), &ContentController::performCommand, this, [this](QUndoCommand* cmd) { m_stack.push(cmd); });

    m_ctrl->setLocalId("localid");

    m_model.reset(new ContentModel());
    m_filteredModel.reset(new FilteredContentModel(Core::ContentType::VECTORIALMAP));

    m_tester.push_back(std::make_unique<QAbstractItemModelTester>(m_model.get()));

    m_filteredModel->setSourceModel(m_model.get());
    m_tester.push_back(std::make_unique<QAbstractItemModelTester>(m_filteredModel.get()));
}

void ContentControllerTest::saveLoadImage()
{
    m_ctrl.reset(new ContentController(new campaign::CampaignManager(nullptr), m_playerCtrl->model(),
                                       m_playerCtrl->characterModel(), nullptr));
    m_ctrl->setLocalId("localid");
    connect(m_ctrl.get(), &ContentController::performCommand, this, [this](QUndoCommand* cmd) { m_stack.push(cmd); });
    {
        auto imgParams
            = std::map<QString, QVariant>({{Core::keys::KEY_UUID, "test_unit_vmap"},
                                           {Core::keys::KEY_PATH, ":/img/girafe3.jpg"},
                                           {Core::keys::KEY_RECT, QRectF(0, 0, 326, 244)},
                                           {Core::keys::KEY_TYPE, QVariant::fromValue(Core::ContentType::VECTORIALMAP)},
                                           {Core::keys::KEY_POSITION, QPointF(500, 200)},
                                           {Core::keys::KEY_TOOL, Core::SelectableTool::IMAGE}});
        auto mapParams= std::map<QString, QVariant>({{"name", QString("Unit Test Map")}});

        m_ctrl->newMedia(nullptr, mapParams);

        auto ctrls= m_ctrl->contentModel()->controllers();

        QCOMPARE(ctrls.size(), 1);

        auto ctrl= ctrls.front();

        auto mapCtrl= dynamic_cast<VectorialMapController*>(ctrl);

        QVERIFY(nullptr != mapCtrl);
        QSignalSpy spy(mapCtrl, &VectorialMapController::visualItemControllerCreated);
        mapCtrl->addItemController(imgParams, true);

        QCOMPARE(spy.count(), 1);
        auto itemCtrl= mapCtrl->itemController("test_unit_vmap");
        QVERIFY(nullptr != itemCtrl);

        auto rectCtrl= dynamic_cast<vmap::ImageItemController*>(itemCtrl);
        QCOMPARE(rectCtrl->rect(), QRectF(0, 0, 326, 244));
        QCOMPARE(rectCtrl->pos(), QPointF(500, 200));
    }

    QCOMPARE(m_ctrl->contentCount(), 1);

    campaign::FileSerializer::writeContentModel(Helper::randomString(), m_ctrl->contentModel());
}

void ContentControllerTest::testFilteredData()
{
    m_model->appendMedia(new VectorialMapController("mapid"));

    auto i= m_model->rowCount();

    QVERIFY(i == 1);

    auto ctrl= m_model->data(m_model->index(0, 0), ContentModel::ControllerRole).value<MediaControllerBase*>();

    QVERIFY(ctrl != nullptr);

    auto vmapCtrl= dynamic_cast<VectorialMapController*>(ctrl);

    QVERIFY(vmapCtrl != nullptr);

    auto j= m_filteredModel->rowCount();

    QVERIFY(j == 1);

    auto base= m_filteredModel->contentController<VectorialMapController*>();

    QVERIFY(base.size() == 1);

    auto first= base.at(0);

    QVERIFY(first != nullptr);
}

void ContentControllerTest::themeTest()
{
    RolisteamTheme theme;
    theme.setUuid(Helper::randomString());
    theme.setBackgroundColor(Helper::randomColor());
    theme.setName(Helper::randomString());
    theme.setCss(Helper::randomString());
    theme.setRemovable(true);
    theme.setBackgroundPosition(Helper::generate(0, 9));
    theme.setBackgroundImage(Helper::imagePath());

    auto obj= IOHelper::themeToObject(&theme);

    auto theme2= IOHelper::jsonToTheme(obj);

    QCOMPARE(theme.uuid(), theme2->uuid());
    QCOMPARE(theme.getBackgroundColor(), theme2->getBackgroundColor());
    QCOMPARE(theme.getName(), theme2->getName());
    QCOMPARE(theme.getCss(), theme2->getCss());
    QCOMPARE(theme.isRemovable(), theme2->isRemovable());
    QCOMPARE(theme.getBackgroundPosition(), theme2->getBackgroundPosition());
    QCOMPARE(theme.getBackgroundImage(), theme2->getBackgroundImage());
}

void ContentControllerTest::modelTest()
{
    QFETCH(Core::ContentType, contentType);
    using DataVec= std::map<QString, QVariant>;
    QFETCH(DataVec, map);

    auto media= Media::MediaFactory::createLocalMedia(Helper::randomString(), contentType, map, QColor(), true);
    map.insert({Core::keys::KEY_TYPE, QVariant::fromValue(contentType)});

    auto model= m_ctrl->contentModel();
    QVERIFY(model->activeMediaId().isNull());
    auto res= model->data(model->index(-1, -1));
    QVERIFY(!res.isValid());

    if(!media)
        QSKIP("media is null");
    auto id= media->uuid();
    media->setActive(true);

    QVERIFY(model->appendMedia(media));
    QVERIFY(!model->activeMediaId().isNull());

    QList<int> roles{ContentModel::NameRole,     ContentModel::TitleRole,       ContentModel::UuidRole,
                     ContentModel::PathRole,     ContentModel::ContentTypeRole, ContentModel::ActiveRole,
                     ContentModel::ModifiedRole, ContentModel::OwnerIdRole,     ContentModel::ControllerRole};
    for(auto i : roles)
    {
        auto var= model->data(model->index(0, 0), i);
    }
    QVERIFY(!model->appendMedia(nullptr));

    QCOMPARE(model->mediaCount(contentType), 1);
    QCOMPARE(model->mediaCount(Core::ContentType::UNKNOWN), 0);

    model->removeMedia(Helper::randomString());
    model->media(Helper::randomString());

    auto media2= model->media(id);
    QVERIFY(media2);
    QVERIFY(model->removeMedia(id));
    QVERIFY(!model->removeMedia(nullptr));

    model->clearData();
}

void ContentControllerTest::modelTest_data()
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

void ContentControllerTest::controllerTest()
{
    m_ctrl->renameMedia(Helper::randomString(), Helper::randomString());

    auto media= Media::MediaFactory::createLocalMedia(Helper::randomString(), Core::ContentType::MINDMAP,
                                                      std::map<QString, QVariant>({{"path", ""}, {"name", ""}}),
                                                      Helper::randomColor(), true);

    auto model= m_ctrl->contentModel();
    model->appendMedia(media);
    auto a= Helper::randomString();
    m_ctrl->renameMedia(media->uuid(), a);

    QVERIFY(media->url().toString().contains(a));

    m_ctrl->copyData();
    m_ctrl->pasteData();

    m_ctrl->canPaste();
    m_ctrl->canCopy();
    m_ctrl->mediaRoot();
    m_ctrl->readData();
    m_ctrl->clearHistory();
    m_ctrl->closeCurrentMedia();
    m_ctrl->maxLengthTabName();
    m_ctrl->shortTitleTab();
    m_ctrl->workspaceFilename();
    m_ctrl->workspaceColor();
    m_ctrl->processMessage(nullptr);
    m_ctrl->processMessage(nullptr);
}

void ContentControllerTest::serializeTest()
{
    QFETCH(Core::ContentType, contentType);
    using DataVec= std::map<QString, QVariant>;
    QFETCH(DataVec, map);

    auto media= Media::MediaFactory::createLocalMedia(Helper::randomString(), contentType, map, QColor(), true);
    map.insert({Core::keys::KEY_TYPE, QVariant::fromValue(contentType)});

    auto byteArray= IOHelper::saveController(media);

    switch(contentType)
    {
    case Core::ContentType::PDF:
    {
        auto pdfController= new PdfController();
        IOHelper::readPdfController(nullptr, byteArray);
        IOHelper::readPdfController(pdfController, byteArray);
        QCOMPARE(media->name(), pdfController->name());
        QCOMPARE(media->uuid(), pdfController->uuid());
        auto pdfArray= IOHelper::saveController(pdfController);
        QCOMPARE(pdfArray, byteArray);
    }
    break;
    case Core::ContentType::VECTORIALMAP:
    {
        auto mapController= new VectorialMapController();
        VectorialMapMessageHelper::readVectorialMapController(nullptr, byteArray);
        VectorialMapMessageHelper::readVectorialMapController(mapController, byteArray);
        QCOMPARE(media->name(), mapController->name());
        QCOMPARE(media->uuid(), mapController->uuid());
        auto pdfArray= IOHelper::saveController(mapController);
        QCOMPARE(pdfArray, byteArray);
    }
    break;
    case Core::ContentType::PICTURE:
    {
        auto imageController= new ImageController();
        IOHelper::readImageController(nullptr, byteArray);
        IOHelper::readImageController(imageController, byteArray);
        QCOMPARE(media->name(), imageController->name());
        QCOMPARE(media->uuid(), imageController->uuid());
        auto pdfArray= IOHelper::saveController(imageController);
        QCOMPARE(pdfArray, byteArray);
    }
    break;
    case Core::ContentType::NOTES:
    {
        auto noteController= new NoteController();
        IOHelper::readNoteController(nullptr, byteArray);
        IOHelper::readNoteController(noteController, byteArray);
        QCOMPARE(media->name(), noteController->name());
        QCOMPARE(media->uuid(), noteController->uuid());
        auto pdfArray= IOHelper::saveController(noteController);
        QCOMPARE(pdfArray, byteArray);
    }
    break;
    case Core::ContentType::CHARACTERSHEET:
    {
        auto sheetController= new CharacterSheetController();
        IOHelper::readCharacterSheetController(nullptr, byteArray);
        IOHelper::readCharacterSheetController(sheetController, byteArray);
        QCOMPARE(media->name(), sheetController->name());
        QCOMPARE(media->uuid(), sheetController->uuid());
        auto pdfArray= IOHelper::saveController(sheetController);
        QCOMPARE(pdfArray, byteArray);
    }
    break;
    case Core::ContentType::SHAREDNOTE:
    {
        auto sharedNoteController= new SharedNoteController();
        IOHelper::readSharedNoteController(nullptr, byteArray);
        IOHelper::readSharedNoteController(sharedNoteController, byteArray);
        QCOMPARE(media->name(), sharedNoteController->name());
        QCOMPARE(media->uuid(), sharedNoteController->uuid());
        auto pdfArray= IOHelper::saveController(sharedNoteController);
        QCOMPARE(pdfArray, byteArray);
    }
    break;
    case Core::ContentType::WEBVIEW:
    {
        auto webviewController= new WebpageController();
        IOHelper::readWebpageController(nullptr, byteArray);
        IOHelper::readWebpageController(webviewController, byteArray);
        QCOMPARE(media->name(), webviewController->name());
        QCOMPARE(media->uuid(), webviewController->uuid());
        auto pdfArray= IOHelper::saveController(webviewController);
        QCOMPARE(pdfArray, byteArray);
    }
    break;
    case Core::ContentType::MINDMAP:
    {
        auto mindmapController= new MindMapController();
        IOHelper::readMindmapController(nullptr, byteArray);
        IOHelper::readMindmapController(mindmapController, byteArray);
        QCOMPARE(media->name(), mindmapController->name());
        QCOMPARE(media->uuid(), mindmapController->uuid());
        auto pdfArray= IOHelper::saveController(mindmapController);
        QCOMPARE(pdfArray, byteArray);
    }
    break;
    case Core::ContentType::INSTANTMESSAGING:
    case Core::ContentType::UNKNOWN:
        break;
    }
}

void ContentControllerTest::serializeTest_data()
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

void ContentControllerTest::playerControllerTest()
{
    auto model= m_playerCtrl->characterModel();
    QVERIFY(model);
    auto states= m_playerCtrl->characterStateModel();
    QVERIFY(!states);
    auto local= m_playerCtrl->localPlayer();
    m_playerCtrl->removePlayer(local);
    auto localId= m_playerCtrl->localPlayerId();
    auto color= m_playerCtrl->localColor();
    QVERIFY(!color.isValid());
    m_playerCtrl->addLocalCharacter();
    m_playerCtrl->removeLocalCharacter(QModelIndex());
}

QTEST_MAIN(ContentControllerTest)

#include "tst_serialization.moc"
