/***************************************************************************
 * Copyright (C) 2018 by Renaud Guezennec                                   *
 * http://www.rolisteam.org/                                                *
 *                                                                          *
 *  This file is part of rcse                                               *
 *                                                                          *
 * rcse is free software; you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by     *
 * the Free Software Foundation; either version 2 of the License, or        *
 * (at your option) any later version.                                      *
 *                                                                          *
 * rcse is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
 * GNU General Public License for more details.                             *
 *                                                                          *
 * You should have received a copy of the GNU General Public License        *
 * along with this program; if not, write to the                            *
 * Free Software Foundation, Inc.,                                          *
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                 *
 ***************************************************************************/
#include "tablecanvasfield.h"
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QJsonArray>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionGraphicsItem>

#include "field.h"
#include "tablefield.h"
#define SQUARE_SIZE 12

HandleItem::HandleItem(QGraphicsObject* parent) : QGraphicsObject(parent)
{
    m_currentMotion= X_AXIS;
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges | QGraphicsItem::ItemIsMovable
             | QGraphicsItem::ItemIsFocusable);
}

HandleItem::~HandleItem() {}

QVariant HandleItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
    if(change == ItemPositionChange && isSelected())
    {
        QPointF newPos= value.toPointF();
        if(m_currentMotion == X_AXIS)
        {
            newPos.setY(pos().y());
        }
        else if(Y_AXIS == m_currentMotion)
        {
            newPos.setX(pos().x());
        }
        if(newPos != value.toPointF())
        {
            return newPos;
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

QRectF HandleItem::boundingRect() const
{
    return QRectF(0, 0, SQUARE_SIZE, SQUARE_SIZE);
}

void HandleItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setPen(Qt::black);
    painter->fillRect(m_startPoint.x(), m_startPoint.y(), SQUARE_SIZE, SQUARE_SIZE, Qt::gray);
    painter->drawRect(m_startPoint.x(), m_startPoint.y(), SQUARE_SIZE, SQUARE_SIZE);
}

void HandleItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsObject::mouseMoveEvent(event);
}

void HandleItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsObject::mouseReleaseEvent(event);
}
void HandleItem::load(QJsonObject& json)
{
    m_currentMotion= static_cast<MOTION>(json["motion"].toInt());
    qreal x= json["x"].toDouble();
    qreal y= json["y"].toDouble();

    qreal posx= json["posx"].toDouble();
    qreal posy= json["posy"].toDouble();

    setPos(posx, posy);
    m_posHasChanged= json["haschanged"].toBool();

    m_startPoint.setX(x);
    m_startPoint.setY(y);
}
void HandleItem::save(QJsonObject& json)
{
    json["motion"]= static_cast<int>(m_currentMotion);
    json["x"]= m_startPoint.x();
    json["y"]= m_startPoint.y();
    json["posx"]= pos().x();
    json["posy"]= pos().y();
    json["haschanged"]= m_posHasChanged;
}

//////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////
bool compareHandles(HandleItem* first, HandleItem* two)
{
    return (first->pos().x() < two->pos().x());
}

TableCanvasField::TableCanvasField(FieldController* field)
    : CanvasField(field), m_colunmCount(1), m_lineCount(1), m_dataReset(true)
{
    m_addColumn= new ButtonCanvas();
    m_addColumn->setParentItem(this);

    m_dialog.reset(new ColumnDefinitionDialog());

    connect(m_dialog.get(), &ColumnDefinitionDialog::columnCountChanged, this, [=](int i) {
        while(i > m_colunmCount)
        {
            addColumn();
        }
        while(i < m_colunmCount)
        {
            removeColumn();
        }
    });

    connect(m_dialog.get(), &ColumnDefinitionDialog::lineCountChanged, this, [=](int i) {
        m_lineCount= i;
        update();
    });

    m_addLine= new ButtonCanvas();
    m_addLine->setParentItem(this);

    m_defineColumns= new QAction(tr("Properties"), this);
    //    m_properties = new QAction(tr("Properties"),this);

    connect(m_defineColumns, &QAction::triggered, this, &TableCanvasField::defineColumns);

    m_addLine->setMsg("+");
    m_addColumn->setMsg("+");

    m_addLine->setPos(24, 200);
    m_addColumn->setPos(200, 24);

    connect(m_addColumn, SIGNAL(clicked()), this, SLOT(addColumn()));
    connect(m_addLine, SIGNAL(clicked()), this, SLOT(addLine()));
}
TableCanvasField::~TableCanvasField() {}
void TableCanvasField::addColumn()
{
    m_colunmCount++;

    HandleItem* item= new HandleItem(this);
    m_handles.append(item);

    qreal colW= boundingRect().width() / (m_handles.size() + 1);
    for(auto item : m_handles)
    {
        item->setPos(colW * (m_handles.indexOf(item) + 1), boundingRect().height() / 2);
    }
    if(m_columnDefined)
    {
        m_dataReset= true;
        defineColumns();
    }
    update();
}
void TableCanvasField::removeColumn()
{
    m_colunmCount--;
    if(m_handles.isEmpty())
        return;

    m_handles.removeLast();

    qreal colW= boundingRect().width() / (m_handles.size() + 1);
    for(auto item : m_handles)
    {
        item->setPos(colW * (m_handles.indexOf(item) + 1), boundingRect().height() / 2);
    }
    if(m_columnDefined)
    {
        m_dataReset= true;
        defineColumns();
    }
    update();
}

void TableCanvasField::addLine()
{
    m_lineCount++;
    update();
}

void TableCanvasField::defineColumns()
{
    std::sort(m_handles.begin(), m_handles.end(), compareHandles);

    if(m_dataReset)
    {
        m_dialog->setData(m_handles, boundingRect().width(), m_lineCount, boundingRect().height());
        m_dataReset= false;
    }

    if(m_dialog->isHidden())
    {
        m_dialog->exec();
        update();
        m_columnDefined= true;
    }
}

void TableCanvasField::setMenu(QMenu& menu)
{
    menu.addAction(m_defineColumns);
}

int TableCanvasField::lineCount() const
{
    return m_lineCount;
}

void TableCanvasField::setLineCount(int lineCount)
{
    m_lineCount= lineCount;
}

int TableCanvasField::colunmCount() const
{
    return m_colunmCount;
}

void TableCanvasField::setColunmCount(int colunmCount)
{
    m_colunmCount= colunmCount;
}

void TableCanvasField::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    if(nullptr == m_field)
        return;
    painter->save();
    painter->fillRect(m_rect, m_field->bgColor());
    painter->setPen(Qt::black);
    painter->drawRect(m_rect);

    m_addLine->setPos(0, boundingRect().height() / 2);
    m_addColumn->setPos(boundingRect().width() / 2, 0);

    if(hasFocusOrChild())
    {
        painter->save();
        QPen pen= painter->pen();
        pen.setColor(Qt::red);
        pen.setWidth(5);
        painter->setPen(pen);
        painter->drawRect(m_rect);
        painter->restore();
    }

    for(auto handle : m_handles)
    {
        painter->drawLine(handle->pos().x(), 0, handle->pos().x(), boundingRect().height());
        handle->setVisible(hasFocusOrChild());
    }

    auto yStep= boundingRect().height() / (m_lineCount);

    if(!m_dataReset)
    {
        auto model= m_dialog->model();
        if(nullptr != model)
        {
            auto list= model->children();
            for(int x= 0; x < std::min(m_colunmCount, list.size()); ++x)
            {
                auto field= list.at(x);
                qreal xPos= 0;
                if(x != 0)
                {
                    xPos= m_handles.at(x - 1)->pos().x();
                }
                qreal xEnd= boundingRect().width() - xPos;
                if(x < m_colunmCount - 1)
                {
                    xEnd= m_handles.at(x)->pos().x() - xPos;
                }
                auto itemType= field->getItemType();
                auto fieldH= boundingRect().height() / m_lineCount;
                for(int y= 0; y < m_lineCount; ++y)
                {
                    QRectF rect(xPos, y * yStep, xEnd, fieldH);
                    QPixmap map= QPixmap(m_pictureMap[itemType]);
                    m_pix= map.scaled(32, 32);
                    if((!m_pix.isNull()) && m_showImageField)
                    {
                        painter->drawPixmap(rect.center(), m_pix, m_pix.rect()); //-m_pix.rect().center()
                    }
                    painter->save();
                    painter->setPen(Qt::green);
                    painter->drawRect(rect);
                    painter->restore();
                }
            }
        }
    }

    for(int i= 0; i < m_lineCount; ++i)
    {
        painter->drawLine(0, i * yStep, boundingRect().width(), i * yStep);
    }
    painter->drawText(QPoint(0, 0), m_field->getId());
    painter->restore();
}
bool TableCanvasField::hasFocusOrChild()
{
    if(isSelected())
    {
        return true;
    }
    else
    {
        if(m_handles.isEmpty())
        {
            return false;
        }
        for(int i= 0; i < m_handles.size(); ++i)
        {
            if((m_handles.at(i) != nullptr) && (m_handles.at(i)->isSelected()))
            {
                return true;
            }
        }
    }

    return false;
}
void TableCanvasField::generateSubFields(QTextStream& out)
{
    auto model= m_dialog->model();
    if(nullptr != model)
    {
        model->generateQML(out, CharacterSheetItem::FieldSec, true);
    }
}

CharacterSheetItem* TableCanvasField::getRoot()
{
    auto model= m_dialog->model();
    Section* sec= nullptr;
    if(nullptr != model)
    {
        sec= model->getRootSection();
    }
    return sec;
}
void TableCanvasField::fillLineModel(LineModel* lineModel, TableField* parent)
{
    auto model= m_dialog->model();
    for(int i= 0; i < m_lineCount; ++i)
    {
        LineFieldItem* line= new LineFieldItem();
        for(CharacterSheetItem* child : model->children())
        {
            auto field= dynamic_cast<FieldController*>(child);
            if(nullptr != field)
            {
                auto newField= new FieldController();
                newField->copyField(field, true, false);
                newField->setParent(parent);
                line->insertField(newField);
            }
        }
        lineModel->insertLine(line);
    }
}

void TableCanvasField::load(QJsonObject& json, EditorController* ctrl)
{
    m_lineCount= json["lineCount"].toInt();
    m_colunmCount= json["colunmCount"].toInt();
    m_dataReset= json["dataReset"].toBool();
    m_columnDefined= json["columnDefined"].toBool();

    QJsonArray handles= json["handles"].toArray();
    for(auto handle : handles)
    {
        auto handleItem= new HandleItem(this);
        auto obj= handle.toObject();
        handleItem->load(obj);
        m_handles.append(handleItem);
    }

    QJsonObject dialog= json["dialog"].toObject();
    m_dialog->load(dialog, ctrl);
}
void TableCanvasField::save(QJsonObject& json)
{
    json["lineCount"]= m_lineCount;
    json["colunmCount"]= m_colunmCount;
    json["dataReset"]= m_dataReset;
    json["columnDefined"]= m_columnDefined;

    QJsonArray handles;
    for(auto handle : m_handles)
    {
        QJsonObject obj;
        handle->save(obj);
        handles.push_back(obj);
    }
    json["handles"]= handles;

    QJsonObject dialog;
    m_dialog->save(dialog);
    json["dialog"]= dialog;
}

//////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////
ButtonCanvas::ButtonCanvas() : m_rect(QRectF(0, 0, SQUARE_SIZE * 2, SQUARE_SIZE * 2))
{
    setAcceptedMouseButtons(Qt::LeftButton);
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
}

QRectF ButtonCanvas::boundingRect() const
{
    return m_rect;
}

void ButtonCanvas::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->save();
    painter->fillRect(boundingRect(), Qt::cyan);
    painter->setPen(Qt::black);
    painter->drawRect(boundingRect());
    painter->drawText(boundingRect(), Qt::AlignCenter, m_msg);
    painter->restore();
}

QString ButtonCanvas::msg() const
{
    return m_msg;
}

void ButtonCanvas::setMsg(const QString& msg)
{
    m_msg= msg;
}

void ButtonCanvas::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        if(boundingRect().contains(event->pos()))
        {
            emit clicked();
        }
    }
    else
    {
        QGraphicsObject::mousePressEvent(event);
    }
}

QRectF ButtonCanvas::rect() const
{
    return m_rect;
}

void ButtonCanvas::setRect(const QRectF& rect)
{
    m_rect= rect;
}
