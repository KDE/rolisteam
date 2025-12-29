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
#include "mindmap/command/removenodecommand.h"
#include "mindmap/data/linkcontroller.h"
#include "mindmap/model/minditemmodel.h"
#include "worker/serializerhelper.h"
#include <algorithm>

namespace mindmap
{

RemoveNodeCommand::RemoveNodeCommand(const std::vector<QPointer<MindItem>>& selection, MindItemModel* nodeModel)
    : m_nodeModel(nodeModel)
{

    m_nodeData= SerializerHelper::serializeMindmapNode(selection);

    std::vector<QPointer<MindItem>> links;
    std::for_each(selection.begin(), selection.end(),
                  [this, &links](const QPointer<MindItem>& node)
                  {
                      auto sublinks= m_nodeModel->sublink(node->id());
                      std::copy(sublinks.begin(), sublinks.end(), std::back_inserter(links));
                  });

    m_linkData= SerializerHelper::serializeMindmapNode(links);

    setText(tr("Remove %1 Item(s)").arg(selection.size()));
}

void RemoveNodeCommand::undo()
{
    // std::for_each(m_selection.begin(), m_selection.end(), [this](MindItem* node) { m_nodeModel->appendItem({node});
    // }); std::for_each(m_links.begin(), m_links.end(), [this](LinkController* link) { m_nodeModel->appendItem({link});
    // });

    m_nodeModel->appendItem(SerializerHelper::readMindmapNode(m_nodeData, m_nodeModel));
    m_nodeModel->appendItem(SerializerHelper::readMindmapNode(m_linkData, m_nodeModel));

    /* std::for_each(m_linkData.begin(), m_linkData.end(),
                   [this](const QJsonValueRef& val)
                   {
                       auto obj= val.toObject();
                       m_nodeModel->appendItem({m_nodeModel->item(obj[mindmap::JSON_MINDITEM_ID].toString())});
                   });*/
}

void RemoveNodeCommand::redo()
{
    std::for_each(m_linkData.begin(), m_linkData.end(),
                  [this](const QJsonValueRef& val)
                  {
                      auto obj= val.toObject();
                      m_nodeModel->removeItem(m_nodeModel->item(obj[mindmap::JSON_MINDITEM_ID].toString()));
                  });
    std::for_each(m_nodeData.begin(), m_nodeData.end(),
                  [this](const QJsonValueRef& val)
                  {
                      auto obj= val.toObject();
                      m_nodeModel->removeItem(m_nodeModel->item(obj[mindmap::JSON_MINDITEM_ID].toString()));
                  });
}
} // namespace mindmap
