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
#ifndef LINKNODE_H
#define LINKNODE_H

#include <QSGFlatColorMaterial>
#include <QSGGeometryNode>

#include "mindmap/data/linkcontroller.h"
#include "mindmap/mindmap_global.h"

namespace mindmap
{

class MINDMAP_EXPORT LinkNode : public QSGGeometryNode
{
public:
    LinkNode();
    void setColor(const QColor& color);
    void update(const QRectF& rect, LinkController::Orientation orient, const QRectF& startBox, const QRectF& endBox);

private:
    QSGFlatColorMaterial m_material;
    QSGGeometry m_geometry;
};
} // namespace mindmap
#endif // LINKNODE_H
