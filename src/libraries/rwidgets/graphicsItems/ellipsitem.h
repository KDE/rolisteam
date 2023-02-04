/***************************************************************************
 *      Copyright (C) 2010 by Renaud Guezennec                             *
 *                                                                         *
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
#ifndef ELLIPSITEM_H
#define ELLIPSITEM_H
#include "rwidgets_global.h"
#include "visualitem.h"
namespace vmap
{
class EllipseController;
}
/**
 * @brief displays an ellipse on maps.
 */
class RWIDGET_EXPORT EllipsItem : public VisualItem
{
public:
    EllipsItem(vmap::EllipseController* ctrl);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget= nullptr) override;
    virtual QPainterPath shape() const override;
    virtual void setNewEnd(const QPointF& nend) override;
    virtual QRectF boundingRect() const override;

    void updateChildPosition() override;

protected:
    void initChildPointItem();

private:
    QPointer<vmap::EllipseController> m_ellipseCtrl;
};

#endif // ELLIPSITEM_H
