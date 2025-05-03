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
#include "controller/item_controllers/rectcontroller.h"

#include "controller/view_controller/vectorialmapcontroller.h"
#include "worker/utilshelper.h"

namespace vmap
{
constexpr int minimalSize{25};
RectController::RectController(const std::map<QString, QVariant>& params, VectorialMapController* ctrl, QObject* parent)
    : VisualItemController(VisualItemController::RECT, params, ctrl, parent)
{
    namespace hu= helper::utils;
    namespace cv= Core::vmapkeys;
    using std::placeholders::_1;

    std::function<void(Core::SelectableTool)> lambda= [this](Core::SelectableTool s)
    {
        m_tool= s;
        m_filled= (m_tool == Core::SelectableTool::FILLRECT);
    };
    std::function<void(bool)> lambda2= [this](bool s)
    {
        m_filled= s;
        m_tool= m_filled ? Core::SelectableTool::FILLRECT : Core::SelectableTool::EMPTYRECT;
    };

    hu::setParamIfAny<Core::SelectableTool>(cv::KEY_TOOL, params, lambda);
    hu::setParamIfAny<bool>(cv::KEY_FILLED, params, lambda2);
    hu::setParamIfAny<quint16>(cv::KEY_PENWIDTH, params, std::bind(&RectController::setPenWidth, this, _1));
    hu::setParamIfAny<QRectF>(cv::KEY_RECT, params, std::bind(&RectController::setRect, this, _1));

    connect(this, &RectController::rectChanged, this, [this] { setModified(); });
    connect(this, &RectController::filledChanged, this, [this] { setModified(); });
    connect(this, &RectController::penWidthChanged, this, [this] { setModified(); });
}

bool RectController::filled() const
{
    return m_filled;
}

QRectF RectController::rect() const
{
    return m_rect;
}

void RectController::setRect(QRectF rect)
{
    if(rect == m_rect)
        return;
    m_rect= rect;
    emit rectChanged();
    m_rectEdited= true;
}

void RectController::setPenWidth(qreal w)
{
    if(qFuzzyCompare(w, m_penWidth))
        return;
    m_penWidth= w;
    emit penWidthChanged();
}

void RectController::setCorner(const QPointF& move, int corner, Core::TransformType tt)
{
    Q_UNUSED(tt)
    if(move.isNull())
        return;

    auto rect= m_rect;
    qreal x2= rect.right();
    qreal y2= rect.bottom();
    qreal x= rect.x();
    qreal y= rect.y();
    switch(corner)
    {
    case TopLeft:
        x+= move.x();
        y+= move.y();
        break;
    case TopRight:
        x2+= move.x();
        y+= move.y();
        break;
    case BottomRight:
        x2+= move.x();
        y2+= move.y();
        break;
    case BottomLeft:
        x+= move.x();
        y2+= move.y();
        break;
    }

    if(std::abs(x2 - x) < minimalSize)
    {
        x2= x + minimalSize;
    }
    if(std::abs(y2 - y) < minimalSize)
    {
        y2= y + minimalSize;
    }

    rect.setCoords(x, y, x2, y2);
    if(!rect.isValid())
        rect= rect.normalized();
    setRect(rect);
}

void RectController::aboutToBeRemoved()
{
    emit removeItem();
}

void RectController::endGeometryChange()
{
    VisualItemController::endGeometryChange();
    if(m_rectEdited)
    {
        auto offset= m_rect.topLeft();
        auto oldScenePos= m_mapToScene(m_rect.topLeft());
        m_rect.translate(offset * -1);
        auto newScenePos= m_mapToScene(m_rect.topLeft());
        auto oldPos= m_pos;
        m_pos= QPointF(oldPos.x() + (oldScenePos.x() - newScenePos.x()),
                       oldPos.y() + (oldScenePos.y() - newScenePos.y()));
        emit posChanged(m_pos);
        emit rectChanged();
        emit posEditFinished();
        emit rectEditFinished();
        m_rectEdited= false;
    }
}

quint16 RectController::penWidth() const
{
    return m_penWidth;
}
} // namespace vmap
