/***************************************************************************
 *	Copyright (C) 2026 by Renaud Guezennec                               *
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
#include "mindmap/command/AddChildToPackageCommand.h"

#include "mindmap/data/packagenode.h"
#include "mindmap/model/minditemmodel.h"

namespace mindmap
{
AddChildToPackageCommand::AddChildToPackageCommand(MindItemModel* nodeModel, const QString& idEnd)
    : m_nodeModel(nodeModel), m_idParent(idEnd)
{
    setText(tr("Add Node to Package"));
}

void AddChildToPackageCommand::undo()
{
    m_nodeModel->removeItem(m_node);
}

void AddChildToPackageCommand::redo()
{
    m_node= dynamic_cast<mindmap::PositionedItem*>(m_nodeModel->createItem(MindItem::NodeType));
    auto pItem= m_nodeModel->positionItem(m_idParent);

    if(!pItem || !m_node)
        return;

    auto pack= dynamic_cast<mindmap::PackageNode*>(pItem);

    if(!pack)
        return;

    m_nodeModel->appendItem({m_node});
    pack->addChild(m_node.get(), false);
}
} // namespace mindmap
