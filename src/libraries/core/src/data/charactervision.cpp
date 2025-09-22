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

#include "data/charactervision.h"

CharacterVision::CharacterVision(QObject* parent) : QObject(parent) {}

CharacterVision::~CharacterVision() {}

void CharacterVision::setAngle(qreal a)
{
    if(a == m_angle)
        return;
    m_angle= a;
    emit angleChanged(m_angle);
    m_changes|= ChangedProperty::ANGLE;
}

void CharacterVision::setRadius(qreal r)
{
    if(r == m_radius)
        return;
    m_radius= r;
    emit radiusChanged(m_radius);
    m_changes|= ChangedProperty::RADIUS;
}

void CharacterVision::setPosition(const QPointF& p)
{
    if(p == m_pos)
        return;
    m_pos= p;
    emit positionChanged(m_pos);
    m_changes|= ChangedProperty::POSITION;
}

void CharacterVision::setShape(CharacterVision::SHAPE s)
{
    if(s == m_shape)
        return;
    m_shape= s;
    emit shapeChanged(m_shape);
}

qreal CharacterVision::angle() const
{
    return m_angle;
}

qreal CharacterVision::radius() const
{
    return m_radius;
}

QPointF CharacterVision::position() const
{
    return m_pos;
}

CharacterVision::SHAPE CharacterVision::shape() const
{
    return m_shape;
}

void CharacterVision::setCornerVisible(bool b)
{
    if(b == m_cornerVisible)
        return;
    m_cornerVisible= b;
    emit cornerVisibleChanged(m_cornerVisible);
}

void CharacterVision::endOfGeometryChanges()
{
    if(m_changes & ChangedProperty::ANGLE)
        emit angleEdited();
    if(m_changes & ChangedProperty::RADIUS)
        emit radiusEdited();
    if(m_changes & ChangedProperty::ROTATION)
        emit rotationEdited();
    if(m_changes & ChangedProperty::POSITION)
        emit positionEdited();
    if(m_changes & ChangedProperty::PATH)
        emit pathEdited();

    m_changes= ChangedProperty::NONE;
}

void CharacterVision::updatePosition() {}
/*void CharacterVision::setCornerPoint(ChildPointItem* b)
{
    m_cornerPoint= b;
}

ChildPointItem* CharacterVision::getCornerPoint()
{
    return m_cornerPoint;
}*/

bool CharacterVision::visible() const
{
    return m_visible;
}

bool CharacterVision::cornerVisible() const
{
    return m_cornerVisible;
}

void CharacterVision::setVisible(bool b)
{
    if(b == m_visible)
        return;
    m_visible= b;
    emit visibleChanged(m_visible);
}

/*void CharacterVision::fill(NetworkMessageWriter* msg)
{
    msg->int8((int)m_shape);
    // msg->real(m_pos.x());
    // msg->real(m_pos.y());

    msg->real(m_radius);
    msg->real(m_angle);
}

void CharacterVision::readMessage(NetworkMessageReader* msg)
{
    m_shape= (CharacterVision::SHAPE)msg->int8();

    // m_pos.setX(msg->real());
    // m_pos.setY(msg->real());
    m_radius= msg->real();
    m_angle= msg->real();
}*/

QPainterPath CharacterVision::path() const
{
    return m_path;
}

void CharacterVision::setPath(QPainterPath newPath)
{
    if(m_path == newPath)
        return;
    m_path= newPath;
    emit pathChanged();
    m_changes|= ChangedProperty::PATH;
}

qreal CharacterVision::rotation() const
{
    return m_rotation;
}

void CharacterVision::setRotation(qreal newRotation)
{
    if(qFuzzyCompare(m_rotation, newRotation))
        return;
    m_rotation= newRotation;
    emit rotationChanged();
    m_changes|= ChangedProperty::ROTATION;
}

qreal CharacterVision::side() const
{
    return m_side;
}

void CharacterVision::setSide(qreal newSide)
{
    if(qFuzzyCompare(m_side, newSide))
        return;
    m_side= newSide;
    emit sideChanged();
}

bool CharacterVision::removed() const
{
    return m_removed;
}

void CharacterVision::setRemoved(bool newRemoved)
{
    if(m_removed == newRemoved)
        return;
    m_removed= newRemoved;
    emit removedChanged();
}
