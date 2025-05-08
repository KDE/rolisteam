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
#include "visualitem.h"

#include <QActionGroup>
#include <QCursor>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QMenu>
#include <QPainter>
#include <QUuid>
#include <cmath>

#include "controller/item_controllers/visualitemcontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"

QColor VisualItem::m_highlightColor= QColor(Qt::red);
QColor VisualItem::m_selectedColor= QColor(Qt::blue);
int VisualItem::m_highlightWidth= 6;

QStringList VisualItem::s_type2NameList
    = QStringList() << QObject::tr("Path") << QObject::tr("Line") << QObject::tr("Ellipse") << QObject::tr("Character")
                    << QObject::tr("Text") << QObject::tr("Rect") << QObject::tr("Rule") << QObject::tr("Image");

VisualItem::VisualItem(vmap::VisualItemController* ctrl) : QGraphicsObject(), m_ctrl(ctrl)
{
    if(!m_ctrl)
        return;

    std::function<QPointF(QPointF)> f= [this](const QPointF& pos) -> QPointF { return mapToScene(pos); };
    m_ctrl->setMapToScene(f);

    connect(m_ctrl, &vmap::VisualItemController::posChanged, this,
            [this]()
            {
                setPos(m_ctrl->pos());
                updateScenePos();
            });
    connect(m_ctrl, &vmap::VisualItemController::rotationChanged, this, [this]() { updateScenePos(); });
    connect(m_ctrl, &vmap::VisualItemController::scenePosChanged, this, [this]() { updateScenePos(); });

    connect(m_ctrl, &vmap::VisualItemController::removeItem, this,
            [this]()
            {
                auto sceneP= scene();
                if(sceneP)
                    sceneP->removeItem(this);
                deleteLater();
            });
    connect(m_ctrl, &vmap::VisualItemController::destroyed, this,
            [this]()
            {
                auto sceneP= scene();
                if(sceneP)
                    sceneP->removeItem(this);
                deleteLater();
            });
    auto func= [this]()
    {
        if(m_ctrl->editable() && !m_ctrl->networkUpdate())
        {
            m_ctrl->setPos(pos());
        }
        m_ctrl->setScenePos(scenePos());
    };
    connect(this, &VisualItem::xChanged, this, func);
    connect(this, &VisualItem::yChanged, this, func);

    connect(m_ctrl, &vmap::VisualItemController::colorChanged, this, [this]() { update(); });
    connect(m_ctrl, &vmap::VisualItemController::editableChanged, this, &VisualItem::updateItemFlags);
    connect(m_ctrl, &vmap::VisualItemController::rotationChanged, this,
            [this]() { applyRotation(m_ctrl->rotation()); });
    connect(m_ctrl, &vmap::VisualItemController::selectedChanged, this, [this](bool b) { setSelected(b); });
    connect(m_ctrl, &vmap::VisualItemController::selectableChanged, this, &VisualItem::updateItemFlags);

    connect(m_ctrl, &vmap::VisualItemController::visibleChanged, this, &VisualItem::evaluateVisible);
    connect(m_ctrl, &vmap::VisualItemController::visibilityChanged, this, &VisualItem::evaluateVisible);
    connect(m_ctrl, &vmap::VisualItemController::opacityChanged, this, [this]() { setOpacity(opacityValue()); });
    connect(m_ctrl, &vmap::VisualItemController::layerChanged, this, [this]() { setOpacity(opacityValue()); });

    init();

    connect(m_ctrl, &vmap::VisualItemController::zOrderChanged, this, [this](qreal z) { setZValue(z); });
}

VisualItem::~VisualItem() {}

void VisualItem::evaluateVisible()
{
    auto localIsGM= m_ctrl->localIsGM();
    auto visibleFromPermission= localIsGM || m_ctrl->visibility() != Core::HIDDEN;
    setVisible(m_ctrl->visible() && visibleFromPermission);
}

void VisualItem::updateScenePos()
{
    if(m_ctrl->scenePos() != scenePos() && m_ctrl->networkUpdate())
    {
        auto rect= m_ctrl->rect();
        auto oldScenePos= m_ctrl->scenePos();
        setTransformOriginPoint(rect.center());
        auto newScenePos= scenePos();
        auto oldPos= pos();
        setPos(QPointF(oldPos.x() + (oldScenePos.x() - newScenePos.x()),
                       oldPos.y() + (oldScenePos.y() - newScenePos.y())));
    }
}
void VisualItem::init()
{
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemUsesExtendedStyleOption, true);
}
vmap::VisualItemController* VisualItem::controller() const
{
    return m_ctrl;
}

vmap::VisualItemController::ItemType VisualItem::getType() const
{
    return m_ctrl->itemType();
}

void VisualItem::updateItemFlags()
{
    GraphicsItemFlags flags= QGraphicsItem::ItemIsFocusable;
    Qt::MouseButtons buttons= Qt::NoButton;

    if(m_ctrl->editable())
    {
        flags
            |= QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges | QGraphicsItem::ItemIsMovable;
        buttons= Qt::AllButtons;
    }

    setAcceptedMouseButtons(buttons);
    setFlags(flags);

    for(auto& itemChild : m_children)
    {
        itemChild->setEditableItem(m_ctrl->editable());
    }
}

QColor VisualItem::color() const
{
    return m_ctrl->color();
}

void VisualItem::setColor(QColor color)
{
    m_ctrl->setColor(color);
}

void VisualItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{

    if(event->modifiers() & Qt::ShiftModifier)
    {
        event->ignore();
        return;
    }
    QGraphicsItem::mousePressEvent(event);
    update();
}
void VisualItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if(event->modifiers() & Qt::ShiftModifier)
    {
        event->ignore();
        return;
    }
    QGraphicsItem::mouseMoveEvent(event);
    update();
}
void VisualItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if(event->modifiers() & Qt::ShiftModifier)
    {
        event->ignore();
        return;
    }
    QGraphicsItem::mouseReleaseEvent(event);
    endOfGeometryChange(ChildPointItem::Moving);
    update();
}

QVariant VisualItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if(change == QGraphicsItem::ItemPositionChange)
    {
        QPointF newPos= computeClosePoint(value.toPointF());
        if(newPos != value.toPointF())
        {
            return newPos;
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

void VisualItem::initialize()
{
    qDebug() << "Initialize";
    setPos(m_ctrl->pos());
    setRotation(m_ctrl->rotation());
    evaluateVisible();
    setOpacity(opacityValue());

    auto b= m_ctrl->networkUpdate();
    m_ctrl->setNetworkUpdate(true);
    updateScenePos();
    m_ctrl->setNetworkUpdate(b);

    updateChildPosition();
}

QString VisualItem::uuid() const
{
    return m_ctrl ? m_ctrl->uuid() : QString();
}

QPointF VisualItem::computeClosePoint(QPointF pos)
{
    if(Qt::AltModifier & QGuiApplication::keyboardModifiers())
    {
        int size= m_ctrl->gridSize();
        pos.setX(std::round(pos.x() / size) * size);
        pos.setY(std::round(pos.y() / size) * size);
    }
    return pos;
}
void VisualItem::keyPressEvent(QKeyEvent* event)
{
    if((event->key() == Qt::Key_Delete) && (isSelected()) && canBeMoved())
    {
        // m_ctrl->rem
    }
    else if((event->key() == Qt::Key_C) && (event->modifiers() == Qt::ControlModifier) && (isSelected()))
    {
        // emit duplicateItem(this);
        event->accept();
    }
    QGraphicsItem::keyPressEvent(event);
}

void VisualItem::resizeContents(const QRectF& rect, int pointId, Core::TransformType transformType)
{
    /*    if(!rect.isValid() || isHoldSize())
        {
            return;
        }
        prepareGeometryChange();
        auto width= m_ctrl->rect().width();
        auto height= m_ctrl->rect().height();
        // sendRectGeometryMsg();
        m_resizing= true;
        m_rect= rect;
        if(transformType == VisualItem::KeepRatio)
        {
            auto hfw= height * rect.width() / width;
            if(hfw > 1)
            {
                m_rect.setTop(-hfw / 2);
                m_rect.setHeight(hfw);
            }
        }

        updateChildPosition();*/
}
void VisualItem::updateChildPosition() {}

void VisualItem::addPromoteItemMenu(QMenu* menu)
{
    for(auto& type : m_promoteTypeList)
    {
        QAction* action= menu->addAction(s_type2NameList[type]);
        action->setData(type);
        connect(action, SIGNAL(triggered()), this, SLOT(promoteItem()));
    }
}
void VisualItem::promoteItem()
{
    QAction* act= qobject_cast<QAction*>(sender());
    if(nullptr != act)
    {
        auto type= static_cast<vmap::VisualItemController::ItemType>(act->data().toInt());
        emit promoteItemTo(this, type);
    }
}

qreal VisualItem::opacityValue()
{
    if(!m_ctrl)
        return 1.0;

    auto isGM= m_ctrl->localIsGM();
    auto isGmLayer= m_ctrl->layer() == Core::Layer::GAMEMASTER_LAYER;

    return isGM ? m_ctrl->opacity() : isGmLayer ? 0.0 : m_ctrl->opacity();
}

void VisualItem::addActionContextMenu(QMenu& menu) {}

bool VisualItem::hasFocusOrChild()
{
    if(!m_ctrl)
        return false;

    if(!m_ctrl->editable())
        return false;

    auto result= isSelected();
    for(auto const& child : std::as_const(m_children))
    {
        if(nullptr == child)
            continue;

        if(child->isSelected())
        {
            result= true;
        }
    }
    return result;
}

bool VisualItem::isLocal() const
{
    return !m_ctrl->remote();
}

void VisualItem::endOfGeometryChange(ChildPointItem::Change change)
{
    if(change != ChildPointItem::Moving)
    {
        auto oldScenePos= scenePos();
        setTransformOriginPoint(m_ctrl->rect().center());
        auto newScenePos= scenePos();
        auto oldPos= pos();
        qDebug() << "VisualItem endOfGeometryChange setPos";
        m_ctrl->setPos(QPointF(oldPos.x() + (oldScenePos.x() - newScenePos.x()),
                               oldPos.y() + (oldScenePos.y() - newScenePos.y())));
    }
    m_ctrl->endGeometryChange();
}

void VisualItem::setModifiers(Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(modifiers)
    /// @brief must be implemented in child classes.
    return;
}
VisualItem* VisualItem::promoteTo(vmap::VisualItemController::ItemType type)
{
    Q_UNUSED(type)
    /// @brief must be implemented in child classes.
    return nullptr;
}

void VisualItem::setChildrenVisible(bool b)
{
    if(!b)
    {
        std::for_each(std::begin(m_children), std::end(m_children),
                      [](ChildPointItem* item) { item->setVisible(false); });
        return;
    }

    if(!canBeMoved())
        return;

    for(auto& item : m_children)
    {
        bool isVision= item->control() == ChildPointItem::Control::Vision;
        bool hasFog= m_ctrl->visibility() == Core::FOGOFWAR;
        auto characterVision= m_ctrl->characterVisionEnabled();
        bool isGeometry= item->control() == ChildPointItem::Control::Geometry;
        item->setVisible(isGeometry || (isVision && hasFog && characterVision));
    }
}

void VisualItem::applyRotation(qreal r)
{
    setRotation(r);
    if(!m_ctrl->networkUpdate() && scenePos() != m_ctrl->scenePos())
    {
        m_ctrl->setScenePos(scenePos());
    }
}

bool VisualItem::canBeMoved() const
{
    return m_ctrl->editable();
}

QColor VisualItem::getHighlightColor()
{
    return m_highlightColor;
}

void VisualItem::setHighlightColor(const QColor& highlightColor)
{
    m_highlightColor= highlightColor;
}

void VisualItem::paintCoord(QPainter* painter)
{
#ifdef QT_DEBUG
    auto p= pos();
    auto rect= boundingRect();
    auto transformCenter= QString("Center: %1x%2").arg(transformOriginPoint().x()).arg(transformOriginPoint().y());
    auto itemPos= QString("pos: %1x%2").arg(p.x()).arg(p.y());
    auto scenePosStr= QString("ScenePos: %1x%2").arg(scenePos().x()).arg(scenePos().y());
    auto itemRect= QString("rect: %1x%2 w:%3 - h:%4").arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());

    auto text= QString("%1 %2 %3 %4").arg(transformCenter, itemPos, scenePosStr, itemRect);
    painter->drawText(QPoint(0, 0), text);
#endif
}

int VisualItem::getHighlightWidth()
{
    return m_highlightWidth;
}

void VisualItem::setHighlightWidth(int highlightWidth)
{
    m_highlightWidth= highlightWidth;
}

void VisualItem::setSize(QSizeF size)
{
    Q_UNUSED(size);
    updateChildPosition();
    update();
}
