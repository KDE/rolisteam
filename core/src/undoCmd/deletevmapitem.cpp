/***************************************************************************
 *	Copyright (C) 2017 by Renaud Guezennec                                 *
 *   https://rolisteam.org/contact                                      *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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
#include "deletevmapitem.h"

#include "controller/view_controller/vectorialmapcontroller.h"
#include "vmap/controller/visualitemcontroller.h"
#include "vmap/manager/visualitemcontrollermanager.h"

DeleteVmapItemCommand::DeleteVmapItemCommand(VectorialMapController* ctrl,
                                             const QList<vmap::VisualItemController*>& list, QUndoCommand* parent)
    : QUndoCommand(parent), m_ctrl(ctrl), m_itemCtrls(list)
{
    setText(QObject::tr("Delete %1 Item(s) From Map %1").arg(list.size()));
}

void DeleteVmapItemCommand::redo()
{
    qInfo() << QStringLiteral("redo command DeleteVmapItemCommand: %1 ").arg(text());

    std::for_each(m_itemCtrls.begin(), m_itemCtrls.end(),
                  [this](vmap::VisualItemController* itemCtrl) { m_ctrl->removeItemController(itemCtrl->uuid()); });
    /*m_vmap->removeItem(m_currentItem);
    m_vmap->removeItemFromData(m_currentItem);
    m_vmap->update();

    if(m_sendToAll)
    {
        NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::DelItem);
        msg.string8(m_vmap->getId());         // id map
        msg.string16(m_currentItem->getId()); // id item
        msg.sendToServer();
    }*/
}

void DeleteVmapItemCommand::undo()
{
    qInfo() << QStringLiteral("undo command DeleteVmapItemCommand: %1 ").arg(text());

    std::for_each(m_itemCtrls.begin(), m_itemCtrls.end(), [this](vmap::VisualItemController* itemCtrl) {
        auto manager= m_ctrl->manager(itemCtrl->tool());
        manager->addController(itemCtrl);
    });
    /*if(nullptr != m_currentItem)
    {
        m_vmap->setFocusItem(m_currentItem);
        m_vmap->QGraphicsScene::addItem(m_currentItem);
        m_currentItem->setVisible(m_visible);
        m_currentItem->updateItemFlags();
        m_vmap->addItemFromData(m_currentItem);
        m_vmap->update();

       NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::AddItem);
        msg.string8(m_vmap->getId());
        msg.uint8(m_currentItem->getType());
        m_currentItem->fillMessage(&msg);
        msg.sendToServer();
    }*/
}
