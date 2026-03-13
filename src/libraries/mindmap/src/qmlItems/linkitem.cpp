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
#include "mindmap/qmlItems/linkitem.h"

#include "mindmap/geometry/linknode.h"

namespace mindmap
{
LinkItem::LinkItem()
{
    setFlag(QQuickItem::ItemHasContents, true);
    // setFlag(QQuickItem::Ite)
    setAntialiasing(true);
    setAcceptedMouseButtons(Qt::LeftButton);

    setWidth(280);
    setHeight(280);
}

PointList LinkItem::points() const
{
    return m_points;
}

void LinkItem::setPoints(const PointList& list)
{
    if(list == m_points)
        return;
    m_points= list;
    emit pointsChanged();
}

QColor LinkItem::color() const
{
    return m_color;
}

void LinkItem::setColor(QColor color)
{
    if(m_color == color)
        return;
    m_color= color;
    emit colorChanged();
    m_colorChanged= true;
    update();
}

void LinkItem::mousePressEvent(QMouseEvent* event)
{
    if(!m_controller)
    {
        event->ignore();
        return;
    }
    QPointF p= event->position(); // click in item coordinates

    if(m_controller->poly().containsPoint(p, Qt::OddEvenFill) && event->button() & Qt::LeftButton)
    {
        emit selected(true);
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void LinkItem::mouseDoubleClickEvent(QMouseEvent* event)
{
    if(!writable())
        return;

    if((event->button() & Qt::LeftButton))
    {
        setEditing(true);
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

bool LinkItem::contains(const QPointF& point) const
{
    return m_controller->poly().containsPoint(point, Qt::OddEvenFill);
}

QSGNode* LinkItem::updatePaintNode(QSGNode* node, QQuickItem::UpdatePaintNodeData*)
{

    LinkNode* link= static_cast<LinkNode*>(node);
    if(!link)
    {
        link= new LinkNode();
        link->setColor(m_color);
    }
    if(m_colorChanged)
    {
        link->setColor(m_color);
        m_colorChanged= false;
    }

    if(m_polyChanged)
    {
        link->updateBox(m_controller->poly());
        m_polyChanged= false;
    }

    if(m_selectionChanged)
    {
        link->updateSelected(m_controller->selected());
        m_selectionChanged= false;
    }

    if(m_controller)
    {
        auto line= link->update(QRectF{0, 0, width(), height()}, m_controller->orientation(), m_controller->startBox(),
                                m_controller->endBox());
        setHorizontalOffset(line.center().x());
        setVerticalOffset(line.center().y());
    }
    return link;
}

qreal LinkItem::horizontalOffset() const
{
    return m_horizontalOffset;
}

qreal LinkItem::verticalOffset() const
{
    return m_verticalOffset;
}
void LinkItem::setHorizontalOffset(qreal r)
{
    if(qFuzzyCompare(r, m_horizontalOffset))
        return;
    m_horizontalOffset= r;
    emit horizontalOffsetChanged();
}
void LinkItem::setVerticalOffset(qreal r)
{
    if(qFuzzyCompare(r, m_verticalOffset))
        return;
    m_verticalOffset= r;
    emit verticalOffsetChanged();
}

LinkController* LinkItem::controller() const
{
    return m_controller;
}

void LinkItem::setController(LinkController* newController)
{
    if(m_controller == newController)
        return;
    m_controller= newController;
    emit controllerChanged();

    connect(m_controller, &LinkController::selectedChanged, this,
            [this]()
            {
                m_selectionChanged= true;
                update();
            });
    connect(m_controller, &LinkController::polyChanged, this,
            [this]()
            {
                m_polyChanged= true;
                update();
            });
}

bool LinkItem::editing() const
{
    return m_editing;
}

void LinkItem::setEditing(bool newEditing)
{
    if(m_editing == newEditing)
        return;
    m_editing= newEditing;
    emit editingChanged();
}

bool LinkItem::writable() const
{
    return m_writable;
}

void LinkItem::setWritable(bool newWritable)
{
    if(m_writable == newWritable)
        return;
    m_writable= newWritable;
    emit writableChanged();
}

} // namespace mindmap
