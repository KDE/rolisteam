/***************************************************************************
 *   Copyright (C) 2011 by Renaud Guezennec                                *
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
#include <QTest>

#include "controller/view_controller/webpagecontroller.h"
#include "data/campaignmanager.h"
#include "diceparser_qobject/diceroller.h"
#include "network/networkmessagereader.h"
#include "updater/media/genericupdater.h"
#include "updater/vmapitem/vmapitemcontrollerupdater.h"
#include <helper.h>
#include <memory>

class WebPageUpdaterTest : public QObject
{
    Q_OBJECT

public:
    WebPageUpdaterTest();

private slots:
    void init();
    void cleanup();
    void updaterTest();

private:
    Helper::TestMessageSender m_sender;
    std::unique_ptr<GenericUpdater> m_updater;
    std::unique_ptr<campaign::CampaignManager> m_campaign;
    std::unique_ptr<WebpageController> m_ctrl;
};

WebPageUpdaterTest::WebPageUpdaterTest() {}

void WebPageUpdaterTest::init()
{
    m_ctrl.reset(new WebpageController);
    m_campaign.reset(new campaign::CampaignManager{new DiceRoller});
    m_updater.reset(new GenericUpdater(m_campaign.get()));
    NetworkMessage::setMessageSender(&m_sender);
}

void WebPageUpdaterTest::cleanup()
{
    auto datas= m_sender.messageData();
    for(auto const& msgData : datas)
    {
        NetworkMessageReader reader;
        reader.setData(msgData);
        m_updater->processMessage(&reader);
    }
}

void WebPageUpdaterTest::updaterTest()
{
    m_updater->addMediaController(nullptr);

    m_ctrl->setHtmlSharing(true);
    m_ctrl->setHtmlSharing(false);
    m_ctrl->setHtmlSharing(true);
    m_ctrl->setHtmlSharing(true);

    m_updater->addMediaController(m_ctrl.get());

    m_ctrl->setModified(true);
    m_ctrl->setModified(false);
    m_ctrl->setModified(true);
    m_ctrl->setModified(true);

    m_ctrl->setSharing(true);
    m_ctrl->setSharing(false);
    m_ctrl->setSharing(true);
    m_ctrl->setSharing(true);

    m_ctrl->setHtmlSharing(true);
    m_ctrl->setHtmlSharing(false);
    m_ctrl->setHtmlSharing(true);
    m_ctrl->setHtmlSharing(true);

    m_ctrl->htmlChanged(Helper::randomString());
    // m_updater->addMediaController();
}

QTEST_MAIN(WebPageUpdaterTest);

#include "tst_webpagetest.moc"
