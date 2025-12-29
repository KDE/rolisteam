/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                                 *
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
#include "mindmap/command/reparentingnodecommand.h"

#include <QDebug>

#include "mindmap/data/linkcontroller.h"
#include "mindmap/data/mindnode.h"
#include "mindmap/model/minditemmodel.h"

namespace mindmap
{
ReparentingNodeCommand::ReparentingNodeCommand(MindItemModel* nodeModel, PositionedItem* newParent, const QString& id)
    : m_nodeModel(nodeModel), m_newParent(newParent), m_nodeId(id)
{

    setText(tr("Reparenting item"));
}

void ReparentingNodeCommand::undo()
{
    reparenting(m_oldParent, m_nodeId);
}

void ReparentingNodeCommand::redo()
{
    m_oldParent= reparenting(m_newParent, m_nodeId);
}

PositionedItem* ReparentingNodeCommand::reparenting(PositionedItem* newParent, const QString& id)
{
    auto mindNode= dynamic_cast<PositionedItem*>(m_nodeModel->item(id));

    if(!mindNode)
        return nullptr;

    auto oldParent= mindNode->parentNode();
    if(!oldParent)
    {
        qWarning() << "Old parent not found";
        oldParent= m_nodeModel->parentNode(m_nodeId);
    }
    LinkController* oldLink= nullptr;
    if(oldParent)
    {
        auto links= oldParent->subLinks();
        auto idxLink= std::find_if(links.begin(), links.end(),
                                   [mindNode](LinkController* link) { return link->end() == mindNode; });
        if(idxLink != links.end())
            oldLink= (*idxLink);
    }

    oldParent->removeLink(oldLink);
    m_nodeModel->removeItem(oldLink);
    auto newLink= new LinkController();
    newLink->setStart(newParent);
    newLink->setEnd(mindNode);
    mindNode->setParentNode(newParent);
    m_nodeModel->appendItem({newLink});

    return oldParent;
}

} // namespace mindmap
