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
#include "lineitem.h"
#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionGraphicsItem>

#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"

#include "controller/view_controller/vectorialmapcontroller.h"
#include "vmap/controller/visualitemcontroller.h"

#include "vmap/controller/linecontroller.h"
#include <QDebug>

#include <math.h>
#define PI 3.14159265

LineItem::LineItem(vmap::LineController* ctrl) : VisualItem(ctrl), m_lineCtrl(ctrl)
{
    auto func= [this]() {
        updateChildPosition();
        update();
    };
    connect(m_lineCtrl, &vmap::LineController::endPointChanged, this, func);
    connect(m_lineCtrl, &vmap::LineController::startPointChanged, this, func);

    for(int i= 0; i <= vmap::LineController::End; ++i)
    {
        ChildPointItem* tmp= new ChildPointItem(m_lineCtrl, i, this);
        tmp->setMotion(ChildPointItem::MOVE);
        m_children.append(tmp);
    }
    updateChildPosition();
}

/*LineItem::LineItem(const QPointF& p, const QColor& penColor, int penSize, QGraphicsItem* parent)
    : VisualItem(penColor, penSize, parent)
{
    m_startPoint= p;
    m_endPoint= p;
    m_rect.setTopLeft(p);
}*/

void LineItem::setNewEnd(const QPointF& nend)
{
    m_lineCtrl->setCorner(nend, 1);
    // m_rect.setBottomRight(nend);
}
QRectF LineItem::boundingRect() const
{
    return m_lineCtrl->rect();
    // return QRectF(m_lineCtrl->startPoint(), m_lineCtrl->endPoint()).normalized();
    // return m_rect.normalized();
}
QPainterPath LineItem::shape() const
{
    QLineF line(m_lineCtrl->startPoint(), m_lineCtrl->endPoint());
    line.setLength(line.length() + m_lineCtrl->penWidth() / 2.0);
    QLineF line2(line.p2(), line.p1());
    line2.setLength(line2.length() + m_lineCtrl->penWidth() / 2.0);
    line.setPoints(line2.p2(), line2.p1());

    QLineF normal= line.normalVector();
    normal.setLength(m_lineCtrl->penWidth() / 2.0);
    auto start= normal.p2();
    auto end= normal.pointAt(-1);
    QLineF normal2= line2.normalVector();
    normal2.setLength(m_lineCtrl->penWidth() / 2.0);
    auto p2= normal2.p2();
    auto p3= normal2.pointAt(-1);

    QPainterPath path;
    path.moveTo(start);
    path.lineTo(p3);
    path.lineTo(p2);
    path.lineTo(end);
    path.lineTo(start);
    return path;
}
void LineItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget)
    painter->save();
    auto pen= painter->pen();
    pen.setColor(m_lineCtrl->color());
    pen.setWidth(m_lineCtrl->penWidth());
    painter->setPen(pen);
    painter->drawLine(m_lineCtrl->startPoint(), m_lineCtrl->endPoint());
    setChildrenVisible(hasFocusOrChild());
    painter->restore();

    if(option->state & QStyle::State_MouseOver || isUnderMouse())
    {
        painter->save();
        auto shapePath= shape();
        QPen pen= painter->pen();
        pen.setColor(m_highlightColor);
        pen.setWidth(m_highlightWidth);
        painter->setPen(pen);
        painter->drawPath(shapePath);
        painter->restore();
    }
}
void LineItem::writeData(QDataStream& out) const
{
    //    out << m_rect;
    out << m_lineCtrl->startPoint();
    out << m_lineCtrl->endPoint();
    out << m_lineCtrl->opacity();
    // out << m_penWidth;
    out << m_lineCtrl->color();
    // out << static_cast<int>(m_layer);
}

void LineItem::readData(QDataStream& in)
{
    //   in >> m_rect;
    // in >> m_lineCtrl->startPoint();
    // in >> m_lineCtrl->endPoint();
    qreal opa= 0;
    in >> opa;
    setOpacity(opa);
    // in >> m_penWidth;
    // in >> m_color;
    int i;
    in >> i;
    // m_layer= static_cast<Core::Layer>(i);
}

void LineItem::fillMessage(NetworkMessageWriter* msg)
{
    /*msg->string16(m_id);
    msg->real(scale());
    msg->real(rotation());
    // msg->uint8((int)m_layer);
    msg->real(zValue());
    msg->real(opacity());

    // rect
      msg->real(m_rect.x());
      msg->real(m_rect.y());
      msg->real(m_rect.width());
      msg->real(m_rect.height());
    // m_startPoint
    msg->real(m_lineCtrl->startPoint().x());
    msg->real(m_lineCtrl->startPoint().y());
    // m_endPoint
    msg->real(m_lineCtrl->endPoint().x());
    msg->real(m_lineCtrl->endPoint().y());
    // pen
    // msg->uint16(m_penWidth);
    // msg->rgb(m_color.rgb());

    msg->real(pos().x());
    msg->real(pos().y());*/
}
void LineItem::readItem(NetworkMessageReader* msg)
{
    /* m_id= msg->string16();
     setScale(msg->real());
     setRotation(msg->real());
     // m_layer= static_cast<Core::Layer>(msg->uint8());
     setZValue(msg->real());
     setOpacity(msg->real());
     // rect
       m_rect.setX(msg->real());
       m_rect.setY(msg->real());
       m_rect.setWidth(msg->real());
       m_rect.setHeight(msg->real());
     // center
     m_lineCtrl->startPoint().setX(msg->real());
     m_lineCtrl->startPoint().setY(msg->real());
     // m_endPoint
     m_lineCtrl->endPoint().setX(msg->real());
     m_lineCtrl->endPoint().setY(msg->real());
     // pen
     //  m_penWidth= msg->uint16();
     // m_color= msg->rgb();

     qreal posx= msg->real();
     qreal posy= msg->real();

     setPos(posx, posy);*/
}
void LineItem::setGeometryPoint(qreal pointId, QPointF& pos)
{
    /*  if(m_holdSize)
          return;

      auto pointInt= static_cast<int>(pointId);
      if(pointInt == 0)
      {
          m_resizing= true;
          m_lineCtrl->setStartPoint(pos);
          //  m_rect.setTopLeft(m_startPoint);
      }
      else if(pointInt == 1)
      {
          m_resizing= true;
          m_endPoint= pos;
          //  m_rect.setBottomRight(m_endPoint);
      }*/
}
void LineItem::setRectSize(qreal x, qreal y, qreal w, qreal h)
{
    /*  m_rect.setX(x);
      m_rect.setY(y);
      m_rect.setWidth(w);
      m_rect.setHeight(h);

      m_startPoint= m_rect.topLeft();
      m_endPoint= m_rect.bottomRight();*/
}
void LineItem::updateChildPosition()
{
    // m_child= new QVector<ChildPointItem*>();

    /*for(int i= 0; i < 2; ++i)
    {
         ChildPointItem* tmp= new ChildPointItem(m_ctrl, i, this);
         tmp->setMotion(ChildPointItem::ALL);
         m_children.append(tmp);
    }*/
    m_children.value(0)->setPos(m_lineCtrl->startPoint());
    m_children.value(0)->setPlacement(ChildPointItem::Center);
    m_children.value(1)->setPos(m_lineCtrl->endPoint());
    m_children.value(1)->setPlacement(ChildPointItem::Center);
}
VisualItem* LineItem::getItemCopy()
{
    return nullptr;
    /* LineItem* line= new LineItem(m_startPoint, m_color, m_penWidth);
     line->setNewEnd(m_endPoint);
     line->setOpacity(opacity());
     line->setScale(scale());
     line->setRotation(rotation());
     line->setZValue(zValue());
     line->setLayer(getLayer());

     return line;*/
}
/*void LineItem::setHoldSize(bool holdSize)
{
    VisualItem::setHoldSize(holdSize);
    for(auto child : m_children)
    {
        auto motion= holdSize ? ChildPointItem::NONE : ChildPointItem::ALL;

        child->setMotion(motion);
    }
}*/
void LineItem::endOfGeometryChange(ChildPointItem::Change change)
{
    if(change == ChildPointItem::Resizing)
    {
        auto oldScenePos= scenePos();
        setTransformOriginPoint(boundingRect().center());
        auto newScenePos= scenePos();
        auto oldPos= pos();
        m_lineCtrl->setPos(QPointF(oldPos.x() + (oldScenePos.x() - newScenePos.x()),
                                   oldPos.y() + (oldScenePos.y() - newScenePos.y())));
    }
    VisualItem::endOfGeometryChange(change);
}
