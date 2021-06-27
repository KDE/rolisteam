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
#ifndef VMAP_PATHITEMCONTROLLER_H
#define VMAP_PATHITEMCONTROLLER_H

#include "visualitemcontroller.h"

#include <QColor>
#include <QPainterPath>

namespace vmap
{
class PathController : public VisualItemController
{
    Q_OBJECT
    Q_PROPERTY(std::vector<QPointF> points READ points WRITE setPoints NOTIFY pointsChanged)
    Q_PROPERTY(quint64 pointCount READ pointCount NOTIFY pointCountChanged)
    Q_PROPERTY(quint16 penWidth READ penWidth NOTIFY penWidthChanged)
    Q_PROPERTY(bool closed READ closed WRITE setClosed NOTIFY closedChanged)
    Q_PROPERTY(bool filled READ filled WRITE setFilled NOTIFY filledChanged)
    Q_PROPERTY(bool penLine READ penLine CONSTANT)

public:
    PathController(const std::map<QString, QVariant>& params, VectorialMapController* ctrl, QObject* parent= nullptr);
    bool filled() const;
    bool closed() const;
    quint16 penWidth() const;
    quint64 pointCount() const;
    const std::vector<QPointF>& points() const;
    QPointF pointAt(quint64 corner) const;
    bool penLine() const;
    void addPoint(const QPointF& po);
    QPainterPath path() const;

    void aboutToBeRemoved() override;
    void endGeometryChange() override;
    void setCorner(const QPointF& move, int corner) override;
    QRectF rect() const override;

    void setPoint(const QPointF& p, int corner);
public slots:
    void setFilled(bool filled);
    void setClosed(bool closed);
    void setPoints(const std::vector<QPointF>& points);

signals:
    void filledChanged(bool filled);
    void closedChanged(bool closed);
    void penWidthChanged(quint16 penWidth);
    void pointCountChanged(int pointCount);
    void positionChanged(int corner, QPointF pos);
    void pointPositionEditFinished(qint64 pointIdx, QPointF pos);
    void pointAdded(const QPointF& point, int idx);
    void pointRemoved(int point);
    void pointsChanged();

private:
    std::vector<QPointF> m_points;
    bool m_filled= false;
    bool m_closed= false;
    bool m_penLine= false;
    quint16 m_penWidth= 15;

    qint64 m_modifiedPointIdx= -1;
    bool m_pointPositonEditing= false;
};
} // namespace vmap

Q_DECLARE_METATYPE(std::vector<QPointF>)

#endif // VMAP_PATHITEMCONTROLLER_H
