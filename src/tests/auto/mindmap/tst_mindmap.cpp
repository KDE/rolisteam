/***************************************************************************
 *	Copyright (C) 2021 by Renaud Guezennec                                 *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
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
#include <QtTest/QtTest>

#include "controller/view_controller/mindmapcontroller.h"
#include "mindmap/controller/selectioncontroller.h"
#include "mindmap/data/linkcontroller.h"
#include "mindmap/data/mindnode.h"
#include "mindmap/model/minditemmodel.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "worker/messagehelper.h"
#include <helper.h>
#include <memory>

#include <QSignalSpy>
#include <QTimer>

class MindMapTest : public QObject
{
    Q_OBJECT
public:
    MindMapTest();

private slots:
    void init();

    void addRemoveImageTest();
    void addItemAndUndoTest();
    void reparentTest();
    void AddLinkTest();


    void selectionCtrl();
    void selectionCtrl_data();

    void removeNodeAndLinkTest();
    void removeNodeAndLinkTest_data();

    void remoteAddTest();
private:
    std::unique_ptr<MindMapController> m_ctrl;
};

void MindMapTest::init()
{
    m_ctrl.reset(new MindMapController("test_id"));
}

MindMapTest::MindMapTest() {}


void MindMapTest::addRemoveImageTest()
{
    auto node = std::make_unique<mindmap::MindNode>();
    auto id = Helper::randomString();
    node->setId(id);
    m_ctrl->addNode({node.get()}, false);

    auto imagePath = Helper::imagePath();
    m_ctrl->addImageFor(id, imagePath);

    QVERIFY(m_ctrl->imgModel()->hasPixmap(id));
    QVERIFY(m_ctrl->canUndo());
    QVERIFY(!m_ctrl->canRedo());

    m_ctrl->undo();

    QVERIFY(!m_ctrl->imgModel()->hasPixmap(id));
    QVERIFY(!m_ctrl->canUndo());
    QVERIFY(m_ctrl->canRedo());

    m_ctrl->redo();

    QVERIFY(m_ctrl->imgModel()->hasPixmap(id));
    QVERIFY(m_ctrl->canUndo());
    QVERIFY(!m_ctrl->canRedo());

    m_ctrl->removeImageFor(id);

    QVERIFY(!m_ctrl->imgModel()->hasPixmap(id));
    QVERIFY(m_ctrl->canUndo());
    QVERIFY(!m_ctrl->canRedo());

    m_ctrl->undo();

    QVERIFY(m_ctrl->imgModel()->hasPixmap(id));
    QVERIFY(m_ctrl->canUndo());
    QVERIFY(m_ctrl->canRedo());

    m_ctrl->redo();
    QVERIFY(!m_ctrl->imgModel()->hasPixmap(id));
}

void MindMapTest::AddLinkTest()
{
    auto node = std::make_unique<mindmap::MindNode>();
    auto id = Helper::randomString();
    node->setId(id);

    auto node2 = std::make_unique<mindmap::MindNode>();
    auto id2 = Helper::randomString();
    node2->setId(id2);


    m_ctrl->addNode({node.get(), node2.get()}, false);

    m_ctrl->addLink(id, id2);

    auto items = m_ctrl->itemModel()->items(mindmap::MindItem::LinkType);
    QCOMPARE(items.size(), 1);
    QVERIFY(m_ctrl->canUndo());

    m_ctrl->undo();

    items = m_ctrl->itemModel()->items(mindmap::MindItem::LinkType);
    QCOMPARE(items.size(), 0);
    QVERIFY(m_ctrl->canRedo());

    m_ctrl->redo();
    items = m_ctrl->itemModel()->items(mindmap::MindItem::LinkType);
    QCOMPARE(items.size(), 1);

}

void MindMapTest::addItemAndUndoTest()
{
    m_ctrl->addNode(QString{});
    QVERIFY(m_ctrl->canUndo());
    QCOMPARE(m_ctrl->itemModel()->rowCount(), 1);

    m_ctrl->undo();

    QVERIFY(m_ctrl->canRedo());
    QCOMPARE(m_ctrl->itemModel()->rowCount(), 0);

    m_ctrl->redo();
    QVERIFY(m_ctrl->canUndo());
    QCOMPARE(m_ctrl->itemModel()->rowCount(), 1);
}

void MindMapTest::reparentTest()
{
    /*auto node = std::make_unique<mindmap::MindNode>();
    auto fatherId = Helper::randomString();
    node->setId(fatherId);

    auto node1 = std::make_unique<mindmap::MindNode>();
    auto child1Id = Helper::randomString();
    node1->setId(child1Id);

    auto node2 = std::make_unique<mindmap::MindNode>();
    auto child2Id = Helper::randomString();
    node2->setId(child2Id);*/

    m_ctrl->addNode(QString());
    auto items = m_ctrl->itemModel()->items(mindmap::MindItem::NodeType);
    QCOMPARE(items.size(), 1);

    auto main = dynamic_cast<mindmap::MindNode*>(items[0]);
    auto mainId = main->id();

    m_ctrl->addNode(mainId);
    items = m_ctrl->itemModel()->items(mindmap::MindItem::NodeType);
    QCOMPARE(items.size(), 2);

    auto second = dynamic_cast<mindmap::MindNode*>(items[1]);
    //auto secondId = second->id();

    m_ctrl->addNode(mainId);
    items = m_ctrl->itemModel()->items(mindmap::MindItem::NodeType);
    QCOMPARE(items.size(), 3);

    auto third = dynamic_cast<mindmap::MindNode*>(items[2]);
    auto thirdId = third->id();

    QCOMPARE(main->subNodeCount(), 2);
    QCOMPARE(second->subNodeCount(), 0);
    QCOMPARE(third->subNodeCount(), 0);
    QCOMPARE(third->parentNode(), main);
    QCOMPARE(second->parentNode(), main);

    m_ctrl->reparenting(second, thirdId);

    QCOMPARE(main->subNodeCount(), 1);
    QCOMPARE(second->subNodeCount(), 0);
    QCOMPARE(third->subNodeCount(), 0);
    QCOMPARE(third->parentNode(), main);
    QCOMPARE(second->parentNode(), main);

    m_ctrl->undo();

    QCOMPARE(main->subNodeCount(), 2);
    QCOMPARE(second->subNodeCount(), 0);
    QCOMPARE(third->subNodeCount(), 0);
    QCOMPARE(third->parentNode(), main);
    QCOMPARE(second->parentNode(), main);
}




void MindMapTest::selectionCtrl()
{
    QFETCH(int, nodeCount);
    auto ctrl= new mindmap::SelectionController();
    auto undo= new QUndoStack();
    ctrl->setUndoStack(undo);

    QVERIFY(!ctrl->enabled());
    QVERIFY(!ctrl->hasSelection());

    QList<QPointF> positions;
    positions.reserve(nodeCount);
    QList<mindmap::MindNode*> nodes;
    for(int i= 0; i < nodeCount; ++i)
    {
        auto node= new mindmap::MindNode();
        node->setText(Helper::randomString());
        QPointF pos(Helper::generate(0, 1000), Helper::generate(0, 1000));
        positions.append(pos);
        node->setPosition(pos);

        ctrl->addToSelection(node);
        nodes.append(node);
    }
    if(nodeCount > 0)
        QVERIFY(ctrl->hasSelection());
    QCOMPARE(ctrl->selectedNodes().size(), nodes.size());

    QPointF move{0, 0};
    for(int i= 0; i < nodeCount; ++i)
    {
        QPointF pos(Helper::generate(0, 100), Helper::generate(0, 100));
        ctrl->movingNode(pos);
        move+= pos;
    }

    auto selection= ctrl->selectedNodes();
    int i= 0;
    for(auto item : selection)
    {
        auto pos= positions[i];
        auto pItem= dynamic_cast<mindmap::PositionedItem*>(item);
        QCOMPARE(pItem->position(), pos + move);
        ++i;
    }

    for(auto item : selection)
    {
        ctrl->removeFromSelection(item);
    }
    QVERIFY(!ctrl->hasSelection());

    for(int i= 0; i < nodeCount; ++i)
    {
        auto node= new mindmap::MindNode();
        node->setText(Helper::randomString());
        QPointF pos(Helper::generate(0, 1000), Helper::generate(0, 1000));
        node->setPosition(pos);
        ctrl->addToSelection(node);
    }
    if(nodeCount > 0)
        QVERIFY(ctrl->hasSelection());
    ctrl->clearSelection();
    QVERIFY(!ctrl->hasSelection());

    while(undo->canUndo())
    {
        undo->undo();
    }

    delete ctrl;
    delete undo;
}

void MindMapTest::selectionCtrl_data()
{
    QTest::addColumn<int>("nodeCount");

    for(int i= 0; i < 100; ++i)
    {
        QTest::addRow("cmd %d", i) << i;
    }
}

void MindMapTest::remoteAddTest()
{
    // auto ctrl= new MindMapController("test_id_tmp");
    // auto spacerCtrl= m_ctrl->spacingController();
    auto itemModel= dynamic_cast<mindmap::MindItemModel*>(m_ctrl->itemModel());

    QSignalSpy itemAdded(itemModel, &mindmap::MindItemModel::rowsInserted);

    auto node1= new mindmap::MindNode();
    node1->setText("Node1");
    node1->setPosition(QPointF(0, 0));

    auto node2= new mindmap::MindNode();
    node2->setText("Node2");
    node2->setPosition(QPointF(0, 0));

    auto link= new mindmap::LinkController();
    link->setStart(node1);
    link->setEnd(node2);

    NetworkMessageWriter msg(NetMsg::MindMapCategory, NetMsg::AddMessage);
    MessageHelper::buildAddItemMessage(msg, {node1, node2}, {link});

    auto dataMsg= msg.data();
    NetworkMessageReader readMsg;
    readMsg.setData(dataMsg);
    MessageHelper::readMindMapAddItem(m_ctrl.get(), &readMsg);

    /*  auto dataMsg1= msg1.getData();
      NetworkMessageReader readMsg1;
      readMsg1.setData(dataMsg1);
      MessageHelper::readAddMindMapNode(m_ctrl.get(), &readMsg1);

      auto dataMsgLink= msgLink.getData();
      NetworkMessageReader readMsgLink;
      readMsgLink.setData(dataMsgLink);
      MessageHelper::readMindMapLink(m_ctrl.get(), &readMsgLink);*/

    QCOMPARE(itemModel->rowCount(), 2);
    QCOMPARE(itemAdded.count(), 2);

    auto links= itemModel->items(mindmap::MindItem::LinkType);
    QCOMPARE(links.size(), 0);

    QTimer timer;
    QSignalSpy spacer(&timer, &QTimer::timeout);
    timer.start(5000);

    spacer.wait(5001);

    auto realNode1= dynamic_cast<mindmap::MindNode*>(itemModel->item(node1->id()));
    auto realNode2= dynamic_cast<mindmap::MindNode*>(itemModel->item(node2->id()));
    auto realLink= dynamic_cast<mindmap::LinkController*>(itemModel->item(link->id()));

    QCOMPARE(qIsNaN(realNode1->position().x()), false);
    QCOMPARE(qIsNaN(realNode1->position().y()), false);
    QCOMPARE(qIsNaN(realNode2->position().x()), false);
    QCOMPARE(qIsNaN(realNode2->position().y()), false);

    // QCOMPARE(node1->position(), realNode1->position());
    // QCOMPARE(node1->boundingRect(), realNode1->boundingRect());
    // QCOMPARE(realLink->startPoint(), link->startPoint());
    // QCOMPARE(realLink->endPoint(), link->endPoint());
    if(realLink)
    {
        QVERIFY(realLink->startPoint() != QPointF());
        QVERIFY(realLink->endPoint() != QPointF());
    }
}
void MindMapTest::removeNodeAndLinkTest()
{
    QFETCH(QList<QList<int>>, nodetree);
    QFETCH(int, nodeCount);
    QFETCH(int, linkCount);
    QFETCH(int, indexToRemove);
    QFETCH(int, restingNodes);
    QFETCH(int, restingLinks);

    auto itemModel= dynamic_cast<mindmap::MindItemModel*>(m_ctrl->itemModel());

    auto& nodes= itemModel->items(mindmap::MindItem::NodeType);

    QCOMPARE(itemModel->rowCount(), 0);

    int d= 0;
    QList<QStringList> idLevels;
    for(const auto& level : nodetree)
    {
        QStringList currentLevelIds;
        int index= 0;
        for(auto c : level)
        {
            for(int i= 0; i < c; ++i)
            {
                if(d == 0)
                {
                    m_ctrl->addNode(QString());
                    auto last= nodes[nodes.size() - 1];

                    currentLevelIds << last->id();
                }
                else
                {
                    // qDebug() << "parentId: "<< c-1 << index;
                    auto parentId= idLevels[idLevels.size() - 1].at(index);
                    m_ctrl->addNode(parentId);
                    auto last= nodes[nodes.size() - 1];
                    currentLevelIds << last->id();
                }
            }
            index++;
        }
        idLevels << currentLevelIds;
        ++d;
    }

    QCOMPARE(itemModel->rowCount(), linkCount + nodeCount);

    m_ctrl->undo();

    QCOMPARE(itemModel->rowCount(), std::max(nodeCount - 1, 0) + std::max(linkCount - 1, 0));

    m_ctrl->redo();

    QCOMPARE(itemModel->rowCount(), nodeCount + linkCount);

    if(indexToRemove < 0)
        return;
    // Remove

    QSignalSpy nodeSpy(itemModel, &mindmap::MindItemModel::rowsAboutToBeRemoved);

    auto selectionCtrl= m_ctrl->selectionController();

    auto last= nodes[indexToRemove];
    selectionCtrl->addToSelection(last);
    m_ctrl->removeSelection();

    QCOMPARE(itemModel->rowCount(), restingNodes + restingLinks);

    QCOMPARE(nodeSpy.count(), nodeCount - restingNodes + linkCount - restingLinks);

    m_ctrl->undo();

    QCOMPARE(itemModel->rowCount(), nodeCount + linkCount);
    nodeSpy.clear();

    m_ctrl->redo();

    QCOMPARE(itemModel->rowCount(), restingNodes + restingLinks);

    QCOMPARE(nodeSpy.count(), nodeCount - restingNodes + linkCount - restingLinks);
}

void MindMapTest::removeNodeAndLinkTest_data()
{
    QTest::addColumn<QList<QList<int>>>("nodetree");
    QTest::addColumn<int>("nodeCount");
    QTest::addColumn<int>("linkCount");
    QTest::addColumn<int>("indexToRemove");
    QTest::addColumn<int>("restingNodes");
    QTest::addColumn<int>("restingLinks");

    QTest::addRow("cmd 1") << QList<QList<int>>() << 0 << 0 << -1 << 0 << 0;
    QTest::addRow("cmd 2") << QList<QList<int>>({{1}}) << 1 << 0 << 0 << 0 << 0;
    QTest::addRow("cmd 3") << QList<QList<int>>({{1}, {1}}) << 2 << 1 << 0 << 1 << 0;
    QTest::addRow("cmd 4") << QList<QList<int>>({{1}, {1}}) << 2 << 1 << 1 << 1 << 0;

    QTest::addRow("cmd 5") << QList<QList<int>>({{1}, {2}}) << 3 << 2 << 0 << 0 << 2; // last was 0
    QTest::addRow("cmd 6") << QList<QList<int>>({{1}, {2}}) << 3 << 2 << 1 << 2 << 1;
    QTest::addRow("cmd 7") << QList<QList<int>>({{1}, {2}}) << 3 << 2 << 2 << 2 << 1;

    QTest::addRow("cmd 8") << QList<QList<int>>({{1}, {1}, {1}}) << 3 << 2 << 0 << 0 << 3; // last was 0
    QTest::addRow("cmd 9") << QList<QList<int>>({{1}, {1}, {1}}) << 3 << 2 << 1 << 1 << 1; // last was 0
    QTest::addRow("cmd 10") << QList<QList<int>>({{1}, {1}, {1}}) << 3 << 2 << 2 << 2 << 1;

    QTest::addRow("cmd 11") << QList<QList<int>>({{1}, {2}, {2, 1}}) << 6 << 5 << 0 << 0 << 8; // last was 0
    QTest::addRow("cmd 12") << QList<QList<int>>({{1}, {2}, {2, 1}}) << 6 << 5 << 1 << 3 << 4; // last was 2
    QTest::addRow("cmd 13") << QList<QList<int>>({{1}, {2}, {2, 1}}) << 6 << 5 << 2 << 4 << 4; // last was 3
    QTest::addRow("cmd 14") << QList<QList<int>>({{1}, {2}, {2, 1}}) << 6 << 5 << 3 << 5 << 4;
    QTest::addRow("cmd 15") << QList<QList<int>>({{1}, {2}, {2, 1}}) << 6 << 5 << 4 << 5 << 4;
    QTest::addRow("cmd 16") << QList<QList<int>>({{1}, {2}, {2, 1}}) << 6 << 5 << 5 << 5 << 4;
}

QTEST_MAIN(MindMapTest);

#include "tst_mindmap.moc"
