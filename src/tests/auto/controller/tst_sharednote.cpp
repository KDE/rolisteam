#include <QTest>

#include <QSignalSpy>
#include <memory>

#include "controller/view_controller/sharednotecontroller.h"
#include "data/campaignmanager.h"
#include "diceparser_qobject/diceroller.h"
#include "helper.h"
#include "model/contentmodel.h"
#include "updater/media/sharednotecontrollerupdater.h"

class SharedNoteControllerTest : public QObject
{
    Q_OBJECT
public:
    SharedNoteControllerTest();

private slots:
    void init();

    void updaterTest();

private:
    std::unique_ptr<SharedNoteController> m_ctrl;
    std::unique_ptr<SharedNoteControllerUpdater> m_updater;
    std::unique_ptr<FilteredContentModel> m_noteModel;
    std::unique_ptr<ContentModel> m_contentModel;
    std::unique_ptr<campaign::CampaignManager> m_campaign;
};

SharedNoteControllerTest::SharedNoteControllerTest() {}

void SharedNoteControllerTest::init()
{
    m_ctrl.reset(new SharedNoteController);
    m_contentModel.reset(new ContentModel);
    m_noteModel.reset(new FilteredContentModel(Core::ContentType::SHAREDNOTE));
    m_noteModel->setSourceModel(m_contentModel.get());
    m_campaign.reset(new campaign::CampaignManager{new DiceRoller});
    m_updater.reset(new SharedNoteControllerUpdater(m_noteModel.get(), m_campaign.get()));
}

void SharedNoteControllerTest::updaterTest()
{
    Helper::TestMessageSender sender;
    NetworkMessage::setMessageSender(&sender);

    m_updater->addSharedNoteController(m_ctrl.get());

    auto id= Helper::randomString();
    m_ctrl->openShareNoteTo(id);
    m_ctrl->closeShareNoteTo(id);
    m_ctrl->setModified(true);
    m_ctrl->setModified(false);
    m_ctrl->setModified(true);
    m_ctrl->setModified(true);

    m_ctrl->setName(Helper::randomString());
    m_ctrl->setText(Helper::randomString());

    m_updater->sendOffPermissionChanged(m_ctrl.get(), true, Helper::randomString());
    m_updater->sendOffPermissionChanged(m_ctrl.get(), false, Helper::randomString());

    auto const& data= sender.messageData();
    for(auto const& msgData : data)
    {
        NetworkMessageReader msg;
        msg.setData(msgData);
        m_updater->processMessage(&msg);
    }
}

QTEST_MAIN(SharedNoteControllerTest);

#include "tst_sharednote.moc"
