/***************************************************************************
 *   Copyright (C) 2011 by Renaud Guezennec                                *
 *   renaud@rolisteam.org                  *
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

#include "controller/view_controller/charactersheetcontroller.h"
#include "controller/view_controller/imagecontroller.h"
#include "controller/view_controller/mediacontrollerbase.h"
#include "controller/view_controller/mindmapcontroller.h"
#include "controller/view_controller/pdfcontroller.h"
#include "controller/view_controller/sharednotecontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "controller/view_controller/webpagecontroller.h"
#include "media/mediafactory.h"
#include "worker/messagehelper.h"
#include <QTest>
#include <helper.h>

using ParamMap= std::map<QString, QVariant>;

class MediaFactoryTest : public QObject
{
    Q_OBJECT
public:
    MediaFactoryTest();

private Q_SLOTS:
    void initTestCase();
    void cleanup();
    void createMediaTest();
    void createMediaTest_data();

private:
    Helper::TestMessageSender m_sender;
};

MediaFactoryTest::MediaFactoryTest() {}

void MediaFactoryTest::initTestCase()
{
    NetworkMessage::setMessageSender(&m_sender);
}

void MediaFactoryTest::cleanup()
{
    m_sender.clear();
}

void MediaFactoryTest::createMediaTest()
{
    QFETCH(Core::ContentType, type);
    QFETCH(ParamMap, params);
    QFETCH(bool, localIsGM);
    QFETCH(bool, validresult);

    auto localId= Helper::randomString();
    auto mediaId= Helper::randomString();
    auto color= Helper::randomColor();
    Media::MediaFactory::setLocalId(localId);
    auto base= Media::MediaFactory::createLocalMedia(mediaId, type, params, color, localIsGM);
    //
    if(!validresult)
    {
        QVERIFY(base == nullptr);
        return;
    }
    QVERIFY(base != nullptr);

    QCOMPARE(base->contentType(), type);
    QCOMPARE(base->uuid(), mediaId);
    QCOMPARE(base->localColor(), color);
    QCOMPARE(base->localId(), localId);
    QCOMPARE(base->localGM(), localIsGM);
    QCOMPARE(base->ownerId(), localId);
    QVERIFY(base->localIsOwner());

    auto localId2= Helper::randomString();
    Media::MediaFactory::setLocalId(localId2);
    switch(type)
    {

    case Core::ContentType::VECTORIALMAP:
        MessageHelper::sendOffVMap(dynamic_cast<VectorialMapController*>(base));
        break;
    case Core::ContentType::PICTURE:
        MessageHelper::sendOffImage(dynamic_cast<ImageController*>(base));
        break;
    case Core::ContentType::NOTES:
    case Core::ContentType::CHARACTERSHEET:
    case Core::ContentType::INSTANTMESSAGING:
        break;
    case Core::ContentType::SHAREDNOTE:
        MessageHelper::shareNotesTo(dynamic_cast<SharedNoteController*>(base), {});
        break;
    case Core::ContentType::PDF:
        MessageHelper::sendOffPdfFile(dynamic_cast<PdfController*>(base));
        break;
    case Core::ContentType::WEBVIEW:
        MessageHelper::shareWebpage(dynamic_cast<WebpageController*>(base));
        break;
    case Core::ContentType::MINDMAP:
        MessageHelper::sendOffMindmapToAll(dynamic_cast<MindMapController*>(base));
        break;
    case Core::ContentType::UNKNOWN:
        break;
    }

    auto networkMsgs= m_sender.messageData();

    if(networkMsgs.isEmpty())
        return;

    for(auto msg : networkMsgs)
    {
        NetworkMessageReader reader;
        reader.setData(msg);
        auto color2= Helper::randomColor();
        auto msgType= static_cast<Core::ContentType>(reader.uint8());
        auto netWorkMsg= Media::MediaFactory::createRemoteMedia(msgType, &reader, color2, false);

        QCOMPARE(netWorkMsg->contentType(), type);
        QCOMPARE(netWorkMsg->uuid(), mediaId);
        QCOMPARE(netWorkMsg->localColor(), color2);
        QCOMPARE(netWorkMsg->localId(), localId2);
        QCOMPARE(netWorkMsg->ownerId(), localId);
        QVERIFY(netWorkMsg->remote());
    }
}

void MediaFactoryTest::createMediaTest_data()
{
    QTest::addColumn<Core::ContentType>("type");
    QTest::addColumn<ParamMap>("params");
    QTest::addColumn<bool>("localIsGM");
    QTest::addColumn<bool>("validresult");

    int i= 0;
    for(auto type : QList<Core::ContentType>{
            Core::ContentType::VECTORIALMAP, Core::ContentType::PICTURE, Core::ContentType::NOTES,
            Core::ContentType::CHARACTERSHEET, Core::ContentType::SHAREDNOTE, Core::ContentType::PDF,
            Core::ContentType::WEBVIEW, Core::ContentType::MINDMAP, Core::ContentType::UNKNOWN})
    {
        QTest::addRow("cmd %i", i++) << type << ParamMap{} << false << false;
        if(type != Core::ContentType::UNKNOWN)
            QTest::addRow("cmd %i", i++) << type << Helper::buildController(type) << true << true;
    }
}

QTEST_MAIN(MediaFactoryTest);

#include "tst_mediafactory.moc"
