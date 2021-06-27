/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
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
#ifndef SIGHTCONTROLLER_H
#define SIGHTCONTROLLER_H

#include <QColor>
#include <QPointer>

#include "characteritemcontroller.h"
#include "data/charactervision.h"
#include "visualitemcontroller.h"

class VectorialMapController;
class CharacterItemControllerManager;
namespace vmap
{
// Controller
class SightController : public VisualItemController
{
    Q_OBJECT
    Q_PROPERTY(bool characterSight READ characterSight WRITE setCharacterSight NOTIFY characterSightChanged)
    Q_PROPERTY(QPainterPath fowPath READ fowPath NOTIFY fowPathChanged)
    Q_PROPERTY(QRectF rect READ rect WRITE setRect NOTIFY rectChanged)
    // Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(int characterCount READ characterCount NOTIFY characterCountChanged) // only playable character
public:
    SightController(VectorialMapController* ctrl, QObject* parent= nullptr);

    void aboutToBeRemoved() override;
    void setCorner(const QPointF& move, int corner) override;
    void endGeometryChange() override;

    int fowItemCount() const;
    int characterCount() const;
    QPainterPath fowPath() const;
    QRectF rect() const override;
    bool characterSight() const;
    bool visible() const override;
    const std::vector<CharacterVisionData> visionData() const;
    const std::vector<std::pair<QPolygonF, bool>>& singularityList() const;

public slots:
    void addPolygon(const QPolygonF& poly, bool mask);
    void setRect(QRectF rect);
    void setCharacterSight(bool b);
    void setVisible(bool vi);

signals:
    void fowPathChanged();
    void rectChanged(QRectF r);
    void characterSightChanged();
    void visibleChanged(bool v);
    void characterCountChanged();

private:
    std::vector<std::pair<QPolygonF, bool>> m_fogSingularityList;
    bool m_characterSight= false;
    CharacterVision::SHAPE m_defaultShape= CharacterVision::ANGLE;
    QRectF m_rect= QRectF(0, 0, 1000, 1000);
};
} // namespace vmap

#endif // SIGHTCONTROLLER_H
