/***************************************************************************
 *   Copyright (C) 2015 by Renaud Guezennec                                *
 *   https://rolisteam.org/contact                   *
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
#include "csitem.h"
#include <QDebug>

int CSItem::m_count= 0;
CSItem::CSItem(QGraphicsItem* parent, bool addCount)
{
    Q_UNUSED(parent);
    if(addCount)
    {
        ++m_count;
    }
}

QColor CSItem::bgColor() const
{
    return m_bgColor;
}

void CSItem::setBgColor(const QColor& bgColor)
{
    m_bgColor= bgColor;
}

QColor CSItem::textColor() const
{
    return m_textColor;
}

void CSItem::setTextColor(const QColor& textColor)
{
    m_textColor= textColor;
}

qreal CSItem::getX() const
{
    return 0;
}

qreal CSItem::getY() const
{
    return 0;
}

qreal CSItem::getWidth() const
{
    return 0;
}

qreal CSItem::getHeight() const
{
    return 0;
}

void CSItem::setX(qreal x)
{
    Q_UNUSED(x);
}

void CSItem::setY(qreal y)
{
    Q_UNUSED(y);
}

void CSItem::setWidth(qreal width)
{
    Q_UNUSED(width);
}

void CSItem::setHeight(qreal height)
{
    Q_UNUSED(height);
}

QRectF CSItem::getRect() const
{
    return m_rect;
}

void CSItem::setRect(const QRectF& rect)
{
    m_rect= rect;
}
CSItem::BorderLine CSItem::border() const
{
    return m_border;
}

void CSItem::setBorder(const CSItem::BorderLine& border)
{
    m_border= border;

    emit askUpdate();
}
void CSItem::resetCount()
{
    m_count= 0;
}
void CSItem::setCount(int i)
{
    m_count= i;
}
