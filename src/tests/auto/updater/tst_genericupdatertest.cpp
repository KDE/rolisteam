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

#include "controller/view_controller/pdfcontroller.h"
#include "data/campaignmanager.h"
#include "diceparser_qobject/diceroller.h"
#include "network/networkmessagereader.h"
#include "updater/media/genericupdater.h"
#include "updater/vmapitem/vmapitemcontrollerupdater.h"
#include <helper.h>
#include <memory>

class GenericUpdaterTest : public QObject
{
    Q_OBJECT

public:
    GenericUpdaterTest();

private slots:
    void init();
    void cleanup();
    void updaterTest();

private:
    Helper::TestMessageSender m_sender;
    std::unique_ptr<GenericUpdater> m_updater;
    std::unique_ptr<campaign::CampaignManager> m_campaign;
};

GenericUpdaterTest::GenericUpdaterTest() {}

void GenericUpdaterTest::init()
{
    m_campaign.reset(new campaign::CampaignManager{new DiceRoller});
    m_updater.reset(new GenericUpdater(m_campaign.get()));
    NetworkMessage::setMessageSender(&m_sender);
}

void GenericUpdaterTest::cleanup()
{
    auto datas= m_sender.messageData();
    for(auto const& msgData : datas)
    {
        NetworkMessageReader reader;
        reader.setData(msgData);
        m_updater->processMessage(&reader);
    }
}

void GenericUpdaterTest::updaterTest()
{
    m_updater->addMediaController(nullptr);

    auto pdf= new PdfController(Helper::randomString());
    m_updater->addMediaController(pdf);

    pdf->setModified(true);
    pdf->setModified(false);
    pdf->setModified(true);
    pdf->setModified(true);

    pdf->setSharing(true);
    pdf->setSharing(false);
    pdf->setSharing(true);
    pdf->setSharing(true);
    // m_updater->addMediaController();
}

QTEST_MAIN(GenericUpdaterTest);

#include "tst_genericupdatertest.moc"
