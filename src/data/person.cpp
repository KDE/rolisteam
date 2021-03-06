/***************************************************************************
    *     Copyright (C) 2009 by Renaud Guezennec                              *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
    *                                                                         *
    *   This program is free software; you can redistribute it and/or modify  *
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
#include "person.h"
#include <QDebug>
Person::Person()
{
    m_state = Qt::Unchecked;
}
Person::Person(QString name,QColor color,QString uri)
    : m_name(name),m_color(color)
{
    m_state = Qt::Unchecked;
    m_avatar.load(uri);
}
const QString& Person::getName() const
{
    return m_name;
}
const QColor& Person::getColor() const
{
    return m_color;
}
const QImage& Person::getAvatar() const
{
    return m_avatar;
}
void Person::setAvatar(QImage& p)
{
    m_avatar=p;
}

void Person::setName(QString& p)
{
    m_name = p;
}
void Person::setColor(QColor& p)
{
    m_color=p;
}
void Person::setState(Qt::CheckState m)
{
    m_state = m;
}

Qt::CheckState Person::checkedState()
{
    return m_state;
}

bool Person::hasAvatar() const
{
    return !m_avatar.isNull();
}
