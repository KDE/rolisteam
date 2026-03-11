/***************************************************************************
 *	Copyright (C) 2022 by Renaud Guezennec                               *
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
#include "mindmap/data/packagenode.h"

#include <QDebug>
namespace mindmap
{

ChildrenModel::ChildrenModel(QObject* parent) : QAbstractListModel(parent) {}

int ChildrenModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return m_internalChildren.size();
}

QVariant ChildrenModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    auto r= m_internalChildren.at(index.row());
    QVariant res;
    switch(role)
    {
    case IdRole:
        res= r->id();
        break;
    case NameRole:
        res= r->text();
        break;
    }
    return res;
}

QHash<int, QByteArray> ChildrenModel::roleNames() const
{
    return {{IdRole, "childId"}, {NameRole, "text"}};
}

const QList<PositionedItem*>& ChildrenModel::children() const
{
    return m_internalChildren;
}

bool ChildrenModel::addChild(PositionedItem* item)
{
    if(m_internalChildren.contains(item) || item == nullptr)
        return false;
    beginInsertRows(QModelIndex(), m_internalChildren.size(), m_internalChildren.size());
    m_internalChildren.append(item);
    endInsertRows();
    return true;
}

bool ChildrenModel::removeChild(const QString& id)
{
    beginResetModel();
    bool res= 0 < erase_if(m_internalChildren,
                           [id](PositionedItem* item)
                           {
                               qDebug() << item->id() << id;
                               return id == item->id();
                           });
    endResetModel();
    return res;
}

bool ChildrenModel::containsChild(const QString& childId) const
{
    auto it= std::find_if(std::begin(m_internalChildren), std::end(m_internalChildren),
                          [childId](mindmap::PositionedItem* item) { return item->id() == childId; });

    return it != std::end(m_internalChildren);
}

// Package

PackageNode::PackageNode(QObject* parent) : PositionedItem{MindItem::PackageType, parent}, m_children(new ChildrenModel)
{
    m_text= tr("Package");
    connect(this, &PackageNode::itemDragged, this,
            [this](const QPointF& motion)
            {
                auto list= m_children->children();
                std::for_each(std::begin(list), std::end(list),
                              [motion](PositionedItem* item)
                              {
                                  if(!item)
                                      return;
                                  auto pos= item->position();
                                  item->setPosition({pos.x() - motion.x(), pos.y() - motion.y()});
                              });
            });

    connect(this, &PackageNode::widthChanged, this, &PackageNode::performLayout);
    connect(this, &PackageNode::heightChanged, this, &PackageNode::performLayout);
    connect(this, &PackageNode::minimumMarginChanged, this, &PackageNode::performLayout);

    connect(this, &PackageNode::visibleChanged, this,
            [this](bool visible)
            {
                auto list= m_children->children();
                std::for_each(std::begin(list), std::end(list),
                              [visible](PositionedItem* item) { item->setVisible(visible); });
            });
}

void PackageNode::addChild(PositionedItem* item, bool isNetwork)
{
    if(item == this)
        return;

    if(m_children->addChild(item))
    {
        performLayout();
        if(!isNetwork)
            emit childAdded(item->id());
    }
}

void PackageNode::removeChild(const QString& id, bool network)
{
    if(m_children->removeChild(id) && !network)
        emit childRemoved(id);
}

bool PackageNode::containsChild(const QString& id) const
{
    return m_children->containsChild(id);
}

const QList<PositionedItem*>& PackageNode::children() const
{
    return m_children->children();
}

void PackageNode::performLayout()
{
    auto children= m_children->children();
    if(children.isEmpty())
        return;

    const int w= width();
    const int itemCount= children.size();

    std::vector<int> widths;
    widths.reserve(itemCount);

    for(auto item : children)
        widths.push_back(item->width());

    auto computeMaxRowWidth= [&](int itemsPerLine)
    {
        int maxRowWidth= 0;
        int rowWidth= 0;
        int count= 0;

        for(auto itemW : widths)
        {
            rowWidth+= itemW;
            count++;

            if(count == itemsPerLine)
            {
                maxRowWidth= std::max(maxRowWidth, rowWidth);
                rowWidth= 0;
                count= 0;
            }
        }

        if(count != 0)
            maxRowWidth= std::max(maxRowWidth, rowWidth);

        return maxRowWidth;
    };

    int itemPerLine= 1;

    while(itemPerLine < itemCount && computeMaxRowWidth(itemPerLine + 1) < w)
    {
        itemPerLine++;
    }

    int maxWidth= computeMaxRowWidth(itemPerLine);
    int margin= (w - maxWidth) / (itemPerLine + 1);

    int currentX= (itemPerLine == itemCount) ? margin : m_minimumMargin;
    const int startLine= currentX;
    int currentY= m_minimumMargin;

    int i= 0;
    qreal maxH= 0.;

    QPointF base= position();

    for(auto item : std::as_const(children))
    {
        item->setPosition({base.x() + currentX, base.y() + currentY});

        currentX+= item->width() + margin;
        maxH= std::max(maxH, item->height());

        ++i;

        if(i == itemPerLine)
        {
            currentY+= maxH + m_minimumMargin;
            currentX= startLine;
            maxH= 0.;
            i= 0;
        }
    }
}

int PackageNode::minimumMargin() const
{
    return m_minimumMargin;
}

void PackageNode::setMinimumMargin(int newMinimumMargin)
{
    if(m_minimumMargin == newMinimumMargin)
        return;
    m_minimumMargin= newMinimumMargin;
    emit minimumMarginChanged();
}

QStringList PackageNode::childrenId() const
{
    auto children= m_children->children();
    if(children.isEmpty())
        return {};

    QStringList res;

    std::transform(std::begin(children), std::end(children), std::back_inserter(res),
                   [](PositionedItem* item) { return item->id(); });

    return res;
}

ChildrenModel* PackageNode::model() const
{
    return m_children.get();
}

} // namespace mindmap
