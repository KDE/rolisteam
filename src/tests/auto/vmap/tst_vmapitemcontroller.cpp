#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QTest>

#include "controller/item_controllers/characteritemcontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "rwidgets/graphicsItems/characteritem.h"

#include <helper.h>
#include <memory>

class CharacterItemCtrlTest : public QObject
{
    Q_OBJECT
public:
    CharacterItemCtrlTest();

private slots:
    void init();
    void propertiesTest();

    void token();

private:
    std::unique_ptr<VectorialMapController> m_vmapCtrl;
    std::unique_ptr<vmap::CharacterItemController> m_ctrl;
};

CharacterItemCtrlTest::CharacterItemCtrlTest() {}
void CharacterItemCtrlTest::init()
{
    m_vmapCtrl.reset(new VectorialMapController("aaa"));

    m_ctrl.reset(new vmap::CharacterItemController({{Core::vmapkeys::KEY_CHARAC_NPC, false}}, m_vmapCtrl.get()));
}
void CharacterItemCtrlTest::propertiesTest()
{
    auto res= Helper::testAllProperties(m_ctrl.get(), {});
    for(const auto& f : std::as_const(res.second))
    {
        qDebug() << f << "unmanaged";
    }
}

void CharacterItemCtrlTest::token()
{
    m_ctrl->setPlayableCharacter(true);
    auto character= m_ctrl->character();

    CharacterItem item(m_ctrl.get());
    CharacterItem itemnull(nullptr);

    {
        QImage image(800, 800, QImage::Format_RGBA8888);
        QPainter painter(&image);
        QStyleOptionGraphicsItem options;
        item.paint(&painter, &options, nullptr);
    }

    {
        QImage image(800, 800, QImage::Format_RGBA8888);
        QPainter painter(&image);
        QStyleOptionGraphicsItem options;
        itemnull.paint(&painter, &options, nullptr);
    }

    character->setAvatar(Helper::imageData(true));
    {
        QImage image(800, 800, QImage::Format_RGBA8888);
        QPainter painter(&image);
        QStyleOptionGraphicsItem options;
        item.paint(&painter, &options, nullptr);
    }

    character->setInitiativeScore(20);

    {
        QImage image(800, 800, QImage::Format_RGBA8888);
        QPainter painter(&image);
        QStyleOptionGraphicsItem options;
        item.paint(&painter, &options, nullptr);
    }

    // end of rendering
    // menu
    auto act= new CharacterAction();
    act->setCommand("1d10");
    act->setName("Attack");
    character->addAction(act);

    m_ctrl->setThumnailRect(Helper::randomRect());
    m_ctrl->setThumnailRect(Helper::randomRect());
    m_ctrl->setThumnailRect(Helper::randomRect());

    CharacterState* state= new CharacterState();
    state->setColor(Helper::randomColor());
    state->setLabel(Helper::randomString());
    state->setId(Helper::randomString());
    QList<CharacterState*> list;
    list.append(state);
    Character::setListOfCharacterState(&list);

    auto shape= new CharacterShape();
    shape->setName(Helper::randomString());
    shape->setImage(QImage(Helper::imagePath(true)));

    character->addShape(shape);
}

QTEST_MAIN(CharacterItemCtrlTest)

#include "tst_vmapitemcontroller.moc"
