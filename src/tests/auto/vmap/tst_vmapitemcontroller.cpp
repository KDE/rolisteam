#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QTest>

#include "controller/item_controllers/characteritemcontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "data/player.h"
#include "model/charactermodel.h"
#include "model/playermodel.h"
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
    void testFunction();

private:
    std::unique_ptr<VectorialMapController> m_vmapCtrl;
    std::unique_ptr<vmap::CharacterItemController> m_ctrl;
    std::unique_ptr<CharacterModel> m_characters;
    std::unique_ptr<PlayerModel> m_players;
};

CharacterItemCtrlTest::CharacterItemCtrlTest() {}
void CharacterItemCtrlTest::init()
{
    m_players.reset(new PlayerModel);
    m_characters.reset(new CharacterModel());
    m_characters->setSourceModel(m_players.get());

    auto id= Helper::randomString();
    auto player= new Player(id, Helper::randomString(), Helper::randomColor(), true);
    auto character= new Character(Helper::randomString(), QColor(Qt::blue), false);
    player->addCharacter(character);

    m_players->addPlayer(player);

    m_vmapCtrl.reset(new VectorialMapController("aaa"));
    CharacterFinder::setPcModel(m_characters.get());
    m_ctrl.reset(new vmap::CharacterItemController(
        {{Core::vmapkeys::KEY_CHARAC_NPC, false}, {Core::vmapkeys::KEY_CHARAC_ID, character->uuid()}},
        m_vmapCtrl.get()));
    // m_ctrl->setCharacter();
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

void CharacterItemCtrlTest::testFunction()
{
    m_ctrl->runInit();
    m_ctrl->cleanInit();
    m_ctrl->setShape(0);
    m_ctrl->cleanShape();
    m_ctrl->setShape(1);
    m_ctrl->runCommand(0);
}

QTEST_MAIN(CharacterItemCtrlTest)

#include "tst_vmapitemcontroller.moc"
