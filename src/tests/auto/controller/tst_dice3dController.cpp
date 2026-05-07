#include <QTest>

#include <QSignalSpy>
#include <memory>

#include "dice3dcontroller.h"
#include "helper.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "updater/controller/dicephysicupdater.h"

class Dice3DControllerTest : public QObject
{
    Q_OBJECT
public:
    Dice3DControllerTest();

private slots:
    void init();

    void updaterTest();

private:
    std::unique_ptr<Dice3DController> m_ctrl;
    std::unique_ptr<DicePhysicUpdater> m_updater;
};

Dice3DControllerTest::Dice3DControllerTest() {}

void Dice3DControllerTest::init()
{
    m_ctrl.reset(new Dice3DController(nullptr));
    m_updater.reset(new DicePhysicUpdater(m_ctrl.get()));
}

void Dice3DControllerTest::updaterTest()
{
    Helper::TestMessageSender sender;
    NetworkMessage::setMessageSender(&sender);
    /*NetworkMessageWriter msg(NetMsg::Dice3DCategory, NetMsg::Roll3DAct);
    msg.byteArray32(Helper::randomData());
    msg.sendToServer();*/

    auto model= m_ctrl->model();
    m_ctrl->setSharedOnline(true);

    model->diceRollChanged(Helper::randomData());

    NetworkMessageReader msgR;
    msgR.setData(sender.messageData().first());
    m_updater->processMessage(&msgR);

    m_ctrl->setSharedOnline(false);
    model->diceRollChanged(Helper::randomData());
}

QTEST_MAIN(Dice3DControllerTest);

#include "tst_dice3dController.moc"
