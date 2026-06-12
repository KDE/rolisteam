#include <QMenu>
#include <QObject>
#include <QSignalSpy>
#include <QTest>
#include <QtCore/QCoreApplication>
#include <set>

#include "controller/item_controllers/linecontroller.h"
#include "controller/item_controllers/textcontroller.h"
#include "controller/item_controllers/visualitemcontroller.h"
#include "controller/item_controllers/vmapitemfactory.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "model/contentmodel.h"
#include "model/vmapitemmodel.h"
#include "rwidgets/customs/vmap.h"
#include "rwidgets/graphicsItems/anchoritem.h"
#include "rwidgets/graphicsItems/highlighteritem.h"
#include "rwidgets/graphicsItems/ruleitem.h"
#include "rwidgets/graphicsItems/textitem.h"
#include "rwidgets/graphicsItems/visualitem.h"
#include "rwidgets/mediacontainers/vmapframe.h"
#include "undoCmd/addvmapitem.h"
#include "updater/media/vmapupdater.h"
#include "updater/vmapitem/textcontrollerupdater.h"
#include "updater/vmapitem/vmapitemcontrollerupdater.h"

#include "helper.h"

constexpr int shortTime{5};
using ParamMap= QList<std::map<QString, QVariant>>;

class FakeItem : public vmap::VisualItemController
{
public:
    FakeItem(VisualItemController::ItemType itemType, VectorialMapController* ctrl)
        : VisualItemController(itemType, {}, ctrl) {};
    void aboutToBeRemoved() {};
    void setCorner(const QPointF& move, int corner, Core::TransformType transformType= Core::TransformType::NoTransform)
    {
        Q_UNUSED(move);
        Q_UNUSED(corner);
        Q_UNUSED(transformType);
    };
    QRectF rect() const { return {}; };
};

class VMapTest : public QObject
{
    Q_OBJECT
public:
    VMapTest();

private slots:
    void init();
    void cleanup();
    void addItems();
    void addItems_data();
    void itemWithNoController();
    void itemWithNoController_data();
    void addNullItems();
    void fogTest();
    void fogTest_data();

    void anchorTest();
    void gridTest();
    void addText();

    void updateProperties();
    void commands();

private:
    std::unique_ptr<VectorialMapController> m_ctrl;
    std::unique_ptr<VMapUpdater> m_updater;
    std::unique_ptr<VMapFrame> m_media;
    std::unique_ptr<QUndoStack> m_stack;
    std::unique_ptr<ContentModel> m_contentModel;
    std::unique_ptr<vmap::VmapItemModel> m_vmapModel;
    std::unique_ptr<FilteredContentModel> m_mapModel;
    QPointer<VectorialMapController> m_pctrl;
    QPointer<VMapFrame> m_pmedia;
};

VMapTest::VMapTest()
{
    std::string duration("3600000"); // 3600 seconds -> 60 min
    QByteArray timeoutDuration(duration.c_str(), static_cast<int>(duration.length()));
    qputenv("QTEST_FUNCTION_TIMEOUT", timeoutDuration);
    { // just for checking ..
        auto result= qgetenv("QTEST_FUNCTION_TIMEOUT");
        qDebug() << "timeout set to:" << result << "ms";
    }
}

void VMapTest::init()
{
    if(m_pmedia)
    {
        m_pmedia->deleteLater();
        // delete m_media.release();
    }

    if(m_pctrl)
    {
        delete m_ctrl.release();
    }

    m_contentModel.reset(new ContentModel());
    m_mapModel.reset(new FilteredContentModel(Core::ContentType::VECTORIALMAP));
    m_mapModel->setSourceModel(m_contentModel.get());

    m_updater.reset(new VMapUpdater(nullptr, m_mapModel.get()));
    m_ctrl.reset(new VectorialMapController());
    m_pctrl= m_ctrl.get();
    m_media.reset(new VMapFrame(m_ctrl.get()));
    m_pmedia= m_media.get();
    m_stack.reset(new QUndoStack);

    connect(m_ctrl.get(), &VectorialMapController::performCommand, this,
            [this](QUndoCommand* cmd) { m_stack->push(cmd); });

    m_vmapModel.reset(new vmap::VmapItemModel());
}
void VMapTest::cleanup() {}

void VMapTest::addNullItems()
{
    // auto map= m_media->map();
    auto model= m_ctrl->model();

    model->appendItemController(nullptr);

    QList<vmap::VisualItemController::ItemType> types{
        vmap::VisualItemController::PATH,    vmap::VisualItemController::LINE,
        vmap::VisualItemController::ELLIPSE, vmap::VisualItemController::CHARACTER,
        vmap::VisualItemController::TEXT,    vmap::VisualItemController::RECT,
        vmap::VisualItemController::RULE,    vmap::VisualItemController::IMAGE,
        vmap::VisualItemController::SIGHT,   vmap::VisualItemController::ANCHOR,
        vmap::VisualItemController::GRID,    vmap::VisualItemController::HIGHLIGHTER};

    for(auto type : types)
        model->appendItemController(new FakeItem(type, m_ctrl.get()));

    std::set<int> allowedRoles(
        {Qt::DisplayRole, vmap::VmapItemModel::IdRole, vmap::VmapItemModel::SelectedRole,
         vmap::VmapItemModel::EditableRole, vmap::VmapItemModel::SelectableRole, vmap::VmapItemModel::VisibleRole,
         vmap::VmapItemModel::OpacityRole, vmap::VmapItemModel::RotationRole, vmap::VmapItemModel::LayerRole,
         vmap::VmapItemModel::PositionRole, vmap::VmapItemModel::LocalGmRole, vmap::VmapItemModel::ColorRole,
         vmap::VmapItemModel::LockedRole, vmap::VmapItemModel::ToolRole, vmap::VmapItemModel::TypeRole,
         vmap::VmapItemModel::InitializedRole, vmap::VmapItemModel::ControllerRole});

    for(auto r : allowedRoles)
        model->data(model->index(0, 0), r);

    model->setModifiedToAllItem();

    auto line= vmap::VmapItemFactory::createVMapItem(
        m_ctrl.get(), Core::SelectableTool::LINE,
        Helper::buildLineController(Helper::randomPoint(), Helper::randomPoint(), Helper::randomPoint()));
    model->appendItemController(line);

    m_ctrl->showTransparentItems();
    line->setOpacity(0);

    model->clearData();
}

std::map<QString, QVariant> buildController(Core::SelectableTool tool, int& count)
{
    std::map<QString, QVariant> map;
    switch(tool) //
    {
    case Core::SelectableTool::FILLRECT:
        map= Helper::buildRectController(true, {0, 0, 200, 200});
        count+= 5;
        break;
    case Core::SelectableTool::LINE:
        map= Helper::buildLineController({100, 100}, {500, 100}, {});
        count+= 3;
        break;
    case Core::SelectableTool::EMPTYELLIPSE:
        map= Helper::buildEllipseController(false, 200., 100., {500., 100.});
        count+= 3;
        break;
    case Core::SelectableTool::EMPTYRECT:
        map= Helper::buildRectController(false, {0, 0, 200, 200}, {300, 200});
        count+= 5;
        break;
    case Core::SelectableTool::FILLEDELLIPSE:
        map= Helper::buildEllipseController(true, 200., 100., {500., 100.});
        count+= 3;
        break;
    case Core::SelectableTool::IMAGE:
        map= Helper::buildImageController(":/img/girafe.jpg", {0, 0, 200, 200}, {150, 200});
        count+= 5;
        break;
    case Core::SelectableTool::TEXT:
        map= Helper::buildTextController(false, "Text without border", {0, 0, 200, 200});
        count+= 6;
        break;
    case Core::SelectableTool::TEXTBORDER:
        map= Helper::buildTextController(true, "Text with border", {0, 0, 200, 200});
        count+= 6;
        break;
    case Core::SelectableTool::PATH:
        map= Helper::buildPathController(true, {{0, 0}, {10, 10}, {20, 0}, {30, 10}}, {0, 0});
        count+= 5;
        break;
    case Core::SelectableTool::PEN:
        map= Helper::buildPenController(false, {{0, 0}, {10, 10}, {20, 0}, {30, 10}}, {0, 0});
        count+= 1;
        break;
    case Core::SelectableTool::NonPlayableCharacter:
        map= Helper::buildTokenController(true, {0, 0});
        count+= 7;
        break;
    default:
        break;
    }
    return map;
}

void VMapTest::addText()
{
    int count;
    auto params= buildController(Core::SelectableTool::TEXT, count);
    TextControllerUpdater updater;
    vmap::TextController textCtrl(params, m_ctrl.get());
    updater.addItemController(&textCtrl);
    textCtrl.setBorderRect(Helper::randomRect());
    TextItem item(&textCtrl);

    item.editText();
}

void VMapTest::addItems()
{
#ifdef FULL_TEST
    QFETCH(ParamMap, params);
    QFETCH(int, expected);

    m_ctrl->setVisibility(Core::ALL);
    m_ctrl->setLocalGM(true);
    m_ctrl->setPermission(Core::PermissionMode::GM_ONLY);
    m_media->setVisible(true);

    QSignalSpy performAction(m_ctrl.get(), &VectorialMapController::performCommand);
    QSignalSpy itemCreated(m_ctrl.get(), &VectorialMapController::visualItemControllerCreated);

    auto map= m_media->map();

    for(auto const& p : params)
    {
        performAction.clear();
        itemCreated.clear();
        m_ctrl->insertItemAt(p);
        performAction.wait(shortTime);
        QCOMPARE(performAction.count(), 1);

        itemCreated.wait(shortTime);
        QCOMPARE(itemCreated.count(), 1);

        auto list= map->items();
        QList<VisualItem*> items;
        std::transform(std::begin(list), std::end(list), std::back_inserter(items),
                       [](QGraphicsItem* item) -> VisualItem*
                       {
                           auto vitem= dynamic_cast<VisualItem*>(item);
                           if(!vitem)
                               return nullptr;
                           else
                           {
                               auto ctrl= vitem->controller();
                               if(ctrl->itemType() == vmap::VisualItemController::GRID
                                  || ctrl->itemType() == vmap::VisualItemController::SIGHT)
                               {
                                   return nullptr;
                               }
                               else
                                   return vitem;
                           }
                       });

        items.removeAll(nullptr);

        if(items.empty())
            continue;

        auto vitem= items.first();
        if(!vitem)
            continue;

        // vitem->setPenWidth(Helper::generate<int>(1, 50));
        auto ctrl= vitem->controller();
        if(!ctrl)
            continue;
        vitem->getType();
        vitem->shape();
        QImage image(800, 800, QImage::Format_RGBA8888);
        QPainter painter(&image);
        QStyleOptionGraphicsItem options;
        vitem->paint(&painter, &options);
        vitem->setNewEnd(Helper::randomPoint());
        QMenu menu;
        vitem->addActionContextMenu(menu);

        vitem->endOfGeometryChange(ChildPointItem::Rotation);
        vitem->endOfGeometryChange(ChildPointItem::Moving);
        vitem->setModifiers(Qt::KeyboardModifier::AltModifier | Qt::KeyboardModifier::ControlModifier);
        ctrl->setRotation(Helper::generate(0., 360.));
        vitem->setSize(QSize{Helper::generate(0, 800), Helper::generate(0, 800)});
        vitem->resizeContents(Helper::randomRect(), Helper::generate(0, 10));

        if(ctrl->itemType() != vmap::VisualItemController::IMAGE)
        {
            auto c= Helper::randomColor();
            vitem->setColor(c);
            QCOMPARE(vitem->color(QPointF()), c);
        }
        {
            auto c= Helper::randomColor();
            vitem->setHighlightColor(c);
            QCOMPARE(vitem->getHighlightColor(), c);
        }

        {
            auto c= Helper::generate<int>(0, 50);
            vitem->setHighlightWidth(c);
            QCOMPARE(vitem->getHighlightWidth(), c);
        }

        vitem->shape();

        QVERIFY(vitem->isLocal());

        QCOMPARE(vitem->promoteTo(vmap::VisualItemController::GRID), nullptr);

        vitem->setNewEnd(Helper::randomPoint());

        vitem->shape();

        QSignalSpy isSelected(ctrl, &vmap::VisualItemController::selectedChanged);

        isSelected.clear();

        ctrl->setSelected(!ctrl->selected());
        ctrl->setSelected(ctrl->selected());

        isSelected.wait(shortTime);

        QCOMPARE(isSelected.count(), 1);

        QSignalSpy canUndo(m_stack.get(), &QUndoStack::canUndoChanged);
        m_ctrl->dupplicateItem(QList<vmap::VisualItemController*>{ctrl});

        canUndo.wait(shortTime);
        QCOMPARE(canUndo.count(), 1);
        m_stack->undo();
    }

    QCOMPARE(map->items().count(), expected);
#endif
}
void VMapTest::addItems_data()
{
#ifdef FULL_TEST
    QTest::addColumn<ParamMap>("params");
    QTest::addColumn<int>("expected");

    QTest::addRow("empty") << ParamMap{} << 2;
    std::vector<Core::SelectableTool> data(
        {Core::SelectableTool::FILLRECT, Core::SelectableTool::LINE, Core::SelectableTool::EMPTYELLIPSE,
         Core::SelectableTool::EMPTYRECT, Core::SelectableTool::FILLEDELLIPSE, Core::SelectableTool::IMAGE,
         Core::SelectableTool::TEXT, Core::SelectableTool::TEXTBORDER, Core::SelectableTool::PEN,
         Core::SelectableTool::PATH, Core::SelectableTool::NonPlayableCharacter});
    QSet<Core::SelectableTool> selectedTools;

    int index= 0;
    for(auto const tool : data)
    {
        selectedTools.insert(tool);
        ParamMap list;
        int count= 2;
        for(auto const sl : selectedTools)
        {
            std::map<QString, QVariant> map;
            map= buildController(sl, count);
            map.insert({QString("color"), QVariant::fromValue(Helper::randomColor())});
            map.insert({Core::vmapkeys::KEY_PENWIDTH, Helper::generate<int>(1, 50)});

            list.append(map);
        }
        QTest::addRow("save %d", ++index) << list << count;
    }
#endif
}
void VMapTest::gridTest()
{
    m_ctrl->setVisibility(Core::ALL);
    m_ctrl->setLocalGM(true);
    m_ctrl->setPermission(Core::PermissionMode::GM_ONLY);
    m_media->setVisible(true);

    auto map= m_media->map();
    auto gridCtrl= m_ctrl->gridController();
    auto gridItem= map->gridItem();

    gridItem->setNewEnd(QPoint());

    gridItem->setVisible(false);
    QVERIFY(!gridItem->isVisible());

    QSignalSpy spyGrid(m_ctrl.get(), &VectorialMapController::gridVisibilityChanged);

    m_ctrl->setGridPattern(Core::GridPattern::SQUARE);
    m_ctrl->setGridAbove(true);
    m_ctrl->setGridVisibility(true);
    spyGrid.wait(10);

    QCOMPARE(spyGrid.count(), 1);

    QSignalSpy gridResize(gridItem, &GridItem::parentChanged);
    QSignalSpy spyGridResize(gridCtrl, &vmap::GridController::rectChanged);

    auto size= m_media->size();
    m_media->resize(size * 1.5);

    spyGridResize.wait(20);
    spyGridResize.clear();
    gridCtrl->setRect(m_media->geometry());

    spyGridResize.wait(10);
    // QCOMPARE(spyGridResize.count(), 1);

    QImage image(800, 800, QImage::Format_RGBA8888);
    QPainter painter(&image);
    QStyleOptionGraphicsItem options;
    gridItem->paint(&painter, &options, nullptr);
}
void VMapTest::itemWithNoController()
{
#ifdef FULL_TEST
    QFETCH(Core::SelectableTool, tool);

    QGraphicsObject* v= nullptr;
    auto pos= Helper::randomPoint();
    if(tool == Core::SelectableTool::HIGHLIGHTER)
    {
        m_ctrl->addHighLighter(Helper::randomPoint());

        auto hitem
            = new HighlighterItem(m_ctrl->preferences(), pos, Helper::generate<int>(1, 100), Helper::randomColor());
        v= hitem;
    }
    else if(tool == Core::SelectableTool::RULE)
    {
        RuleItem* item= new RuleItem(m_ctrl.get());
        item->setNewEnd(Helper::randomPoint(), false);
        item->setNewEnd(Helper::randomPoint(), true);
        v= item;

        QList<Core::ScaleUnit> list{Core::M,    Core::KM,   Core::CM,   Core::MILE,
                                    Core::YARD, Core::INCH, Core::FEET, Core::PX};
        for(auto i : list)
        {
            m_ctrl->setScaleUnit(i);
            QImage image(800, 800, QImage::Format_RGBA8888);
            QPainter painter(&image);
            QStyleOptionGraphicsItem options;
            v->paint(&painter, &options);
        }
    }
    auto map= m_media->map();
    map->addItem(v);
    v->setPos(pos);

    v->shape();
    QImage image(800, 800, QImage::Format_RGBA8888);
    QPainter painter(&image);
    QStyleOptionGraphicsItem options;
    v->paint(&painter, &options);
#endif
}

void VMapTest::itemWithNoController_data()
{
#ifdef FULL_TEST
    QTest::addColumn<Core::SelectableTool>("tool");
    QTest::addRow("save %d", 2) << Core::SelectableTool::RULE;
    QTest::addRow("save %d", 6) << Core::SelectableTool::HIGHLIGHTER;
#endif
}

void VMapTest::updateProperties()
{
    Helper::TestMessageSender sender;
    NetworkMessage::setMessageSender(&sender);
    sender.clear();
    m_contentModel->appendMedia(m_ctrl.get());
    m_updater->addMediaController(m_ctrl.get());

    Helper::testAllProperties(m_ctrl.get(), {}, true);

    std::vector<Core::SelectableTool> types(
        {Core::SelectableTool::FILLRECT, Core::SelectableTool::LINE, Core::SelectableTool::EMPTYELLIPSE,
         Core::SelectableTool::EMPTYRECT, Core::SelectableTool::FILLEDELLIPSE, Core::SelectableTool::IMAGE,
         Core::SelectableTool::TEXT, Core::SelectableTool::TEXTBORDER, Core::SelectableTool::PEN,
         Core::SelectableTool::PATH, Core::SelectableTool::NonPlayableCharacter});
    auto count= 0;
    int i= 0;
    m_ctrl->addHighLighter({0, 0});

    for(auto type : types)
    {
        auto map= buildController(type, count);
        auto id= QString("ID_%1").arg(i);
        map.insert({Core::vmapkeys::KEY_UUID, id});
        m_ctrl->insertItemAt(map);

        auto item= m_ctrl->itemController(id);

        item->setCorner(QPointF{100.0, 100.0}, 2);

        qDebug() << Helper::testAllProperties(item, {"borderRect"}, true);
        ++i;
    }
    m_ctrl->removeItemController({QString("ID_0")});
    auto list= sender.messageData();
    for(auto const& msg : std::as_const(list))
    {
        NetworkMessageReader reader;
        reader.setData(msg);
        m_updater->processMessage(&reader);
    }

    m_ctrl.release();
}

void VMapTest::commands()
{
    int count= 0;
    auto map= buildController(Core::SelectableTool::FILLRECT, count);
    auto id= QString("ID_0");
    auto model= m_ctrl->model();
    auto childrenCount= model->rowCount();
    m_ctrl->setToolColor(Qt::red);
    // QSignalSpy spy(m_stack.get(), &QUndoStack::indexChanged);

    map.insert({Core::vmapkeys::KEY_UUID, id});
    map.insert({Core::vmapkeys::KEY_COLOR, m_ctrl->toolColor()});
    m_ctrl->insertItemAt(map);
    // spy.wait();

    QCOMPARE(model->rowCount(), childrenCount + 1);
    auto item= m_ctrl->itemController(id);
    connect(item, &QObject::destroyed, this,
            []()
            {
                qDebug() << "item destroyed";
                // stran
            });
    item->setCorner(QPointF{100.0, 100.0}, 2);

    QCOMPARE(item->color(), Qt::red);
    m_ctrl->setToolColor(Qt::green);
    m_ctrl->askForColorChange(item);
    QCOMPARE(item->color(), Qt::green);
    m_stack->undo();

    QCOMPARE(item->color(), Qt::red);

    m_ctrl->dupplicateItem({item});
    // spy.wait();

    QCOMPARE(model->rowCount(), childrenCount + 2);
    m_stack->undo();
    QCOMPARE(model->rowCount(), childrenCount + 1);

    {
        auto map= buildController(Core::SelectableTool::FILLRECT, count);
        auto id_fog= QString("ID_FOG");
        map.insert({Core::vmapkeys::KEY_UUID, id_fog});
        // auto itemFog= m_ctrl->itemController(id_fog);
        // itemFog->setCorner(QPointF{100.0, 100.0}, 2);

        m_ctrl->changeFogOfWar({{0, 0}, {0., 100}, {100, 100}, {100, 0}}, nullptr, false, false);
    }
    m_stack->undo();

    item->setOpacity(0.0);
    m_ctrl->showTransparentItems();
    QCOMPARE(item->opacity(), 1.0);
    m_stack->undo();
    QCOMPARE(item->opacity(), 0.);
    item->setOpacity(1);

    m_ctrl->hideOtherLayers(true);
    m_stack->undo();

    m_ctrl->hideOtherLayers(false);
    m_stack->undo();

    {
        auto map= buildController(Core::SelectableTool::NonPlayableCharacter, count);
        map.insert({Core::vmapkeys::KEY_UUID, "NPC_1"});
        m_ctrl->insertItemAt(map);
    }

    m_ctrl->rollInit(Core::CharacterScope::AllCharacter);
    m_stack->undo();

    m_ctrl->cleanUpInit(Core::CharacterScope::AllCharacter);
    m_stack->undo();

    auto id2= QString("ID_1");
    {
        auto map= buildController(Core::SelectableTool::FILLRECT, count);

        map.insert({Core::vmapkeys::KEY_UUID, id2});
        m_ctrl->setToolColor(Qt::red);
        m_ctrl->insertItemAt(map);
    }
    auto item2= m_ctrl->itemController(id2);

    m_ctrl->setParent(item, item2);
    m_stack->undo();

    m_ctrl->stackBefore({item->uuid()}, {item2->uuid()});
    m_stack->undo();

    QCOMPARE(model->rowCount(), childrenCount + 2);
    m_ctrl->aboutToRemove({item});
    QVERIFY(item->removed());
    m_stack->undo();
    QVERIFY(!item->removed());
}

void VMapTest::fogTest()
{
    QFETCH(Core::VisibilityMode, mode);
    QFETCH(bool, visibility);

    auto map= m_media->map();
    auto fog= map->sightItem();

    m_ctrl->setVisibility(mode);
    QCOMPARE(fog->isVisible(), visibility);

    fog->setNewEnd(Helper::randomPoint());
    auto p= Helper::randomPoint();
    fog->moveVision(Helper::generate<qreal>(1., 3.), p);

    {
        QImage image(800, 800, QImage::Format_RGBA8888);
        QPainter painter(&image);
        QStyleOptionGraphicsItem options;
        fog->paint(&painter, &options, nullptr);
    }
    auto sctrl= m_ctrl->sightController();
    auto poly= Helper::randomPolygon();
    sctrl->addPolygon(poly, false, false);

    QImage image(800, 800, QImage::Format_RGBA8888);
    QPainter painter(&image);
    QStyleOptionGraphicsItem options;
    fog->paint(&painter, &options, nullptr);
}

void VMapTest::fogTest_data()
{
    QTest::addColumn<Core::VisibilityMode>("mode");
    QTest::addColumn<bool>("visibility");

    QTest::addRow("Hidden") << Core::HIDDEN << false;
    QTest::addRow("FogOfWar") << Core::FOGOFWAR << true;
    QTest::addRow("All") << Core::ALL << false;
}

void VMapTest::anchorTest()
{
    m_ctrl->setVisibility(Core::ALL);
    m_ctrl->setLocalGM(true);
    m_ctrl->setPermission(Core::PermissionMode::GM_ONLY);
    m_media->setVisible(true);

    auto anchor= new AnchorItem();
    anchor->setNewEnd(QPointF());

    anchor->setPos(QPointF(100., 100.));
    anchor->setNewEnd(QPointF(200., 250.));

    auto map= m_media->map();
    map->addItem(anchor);
    anchor->setVisible(true);

    anchor->shape();
    QImage image(800, 800, QImage::Format_RGBA8888);
    QPainter painter(&image);
    QStyleOptionGraphicsItem options;
    anchor->paint(&painter, &options);

    anchor->boundingRect();

    QCOMPARE(anchor->getEnd(), QPointF(300., 350.));
    QCOMPARE(anchor->getStart(), QPointF(100., 100.));

    auto items= map->items();

    QCOMPARE(items.count(), 3);
}

QTEST_MAIN(VMapTest);

#include "tst_vmap.moc"
