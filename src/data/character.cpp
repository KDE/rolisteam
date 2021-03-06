/***************************************************************************
    *     Copyright (C) 2009 by Renaud Guezennec                             *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
    *                                                                         *
    *   This program is free software; you can redistribute it and/or modify     *
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
#include <QBrush>

#include "character.h"



Character::Character()
    : Person()
{
    
}

Character::Character(QString name,QColor color,QString uri)
    : Person(name,color,uri )
{
    
}
Character::Character(const Character& p)
    : Person()
{
    Person::m_name = p.getName();
    Person::m_color = p.getColor();
}

bool Character::isLeaf() const
{
    return true;
}

void Character::setParent(Person* person)
{
    m_parent = person;
}
void Character::setAvatar(QImage &p)
{
    Person::setAvatar(p);
    emit avatarChanged();
}

QDataStream& operator<<(QDataStream& out, const Character& con)
{
    out << con.getName();
    out << con.getColor();
    out << con.getAvatar();
    return out;
}

QDataStream& operator>>(QDataStream& is,Character& con)
{
    is >>(con.m_name);
    is >>(con.m_color);
    is >>(con.m_avatar);
    return is;
}
