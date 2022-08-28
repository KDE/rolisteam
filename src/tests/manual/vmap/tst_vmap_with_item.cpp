#include <QApplication>

#include <QUndoStack>
#include <QVariant>

#include "controller/item_controllers/characteritemcontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "data/character.h"
#include "media/mediatype.h"
#include "rwidgets/mediacontainers/vmapframe.h"
#include "utils/iohelper.h"
#include "worker/iohelper.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    Q_INIT_RESOURCE(viewsqml);
    Q_INIT_RESOURCE(textedit);
    Q_INIT_RESOURCE(rolisteam);
    Q_INIT_RESOURCE(resources);

    QIcon::setFallbackSearchPaths(QIcon::fallbackSearchPaths() << ":/resources/rolistheme");

    QList<CharacterState*> states;
    auto cs1= new CharacterState();
    cs1->setId("ii");
    cs1->setLabel("State 1");
    cs1->setColor(Qt::green);
    cs1->setImagePath(":/img/arbre_square_500.jpg");

    states << cs1;

    auto cs2= new CharacterState();
    cs2->setId("iO");
    cs2->setLabel("State 2");
    cs2->setColor(Qt::blue);
    cs2->setImagePath(":/img/lion.jpg");

    states << cs2;

    Character::setListOfCharacterState(&states);

    QUndoStack undoStack;

    VectorialMapController ctrl;
    ctrl.setLocalColor(Qt::cyan);
    QObject::connect(
        &ctrl, &VectorialMapController::performCommand, [&undoStack](QUndoCommand* cmd) { undoStack.push(cmd); });
    ctrl.setLocalGM(true);
    ctrl.setActive(true);
    // ctrl.setVisibility(Core::ALL);
    //  ctrl.setEditionMode(Core::EditionMode::Mask);
    //  ctrl.setPermission(Core::PC_ALL);
    ctrl.setLocalId("uuidI");
    ctrl.setOwnerId("uuid");

    Character character("aaa", "Lynn Gray-Rike", QColor("#0000DD"), false, 0);
    character.setAvatar(utils::IOHelper::loadFile(
        "/home/renaud/documents/03_jdr/01_Scenariotheque/09_Cops/03_krom/lynn_gray_rike.jpg"));

    QList<CharacterAction*> acts;

    character.defineActionList(acts);
    QPointF pos{200, 200};

    std::map<QString, QVariant> params;
    params.insert({Core::vmapkeys::KEY_CHARACTER, QVariant::fromValue(&character)});
    params.insert({Core::vmapkeys::KEY_POS, pos});
    params.insert({Core::vmapkeys::KEY_TOOL, Core::SelectableTool::PlayableCharacter});

    auto c= new vmap::CharacterItemController(params, &ctrl);
    VMapFrame frame(&ctrl);

    frame.setVisible(true);
    ctrl.addRemoteItem(c);

    return app.exec();
}
