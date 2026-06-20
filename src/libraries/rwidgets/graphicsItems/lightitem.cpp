#include "lightitem.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QRadialGradient>
#include <QStyleOptionGraphicsItem>

#include "controller/item_controllers/lightcontroller.h"
namespace
{
constexpr qreal CENTER_RADIUS{8.0}; // hit box / center dot radius
}
QRectF computeBoundingRect(const qreal r)
{
    return QRectF(-r, -r, r * 2, r * 2);
}

LightItem::LightItem(vmap::LightController* ctrl) : VisualItem(ctrl), m_lightCtrl(ctrl)
{
    auto updateFunc= [this]() { updateChildPosition(); };
    connect(m_lightCtrl, &vmap::LightController::radiusChanged, this, updateFunc);
    connect(this, &LightItem::parentChanged, this,
            [this]
            {
                disconnect(m_connect);

                auto p= dynamic_cast<VisualItem*>(parentItem());
                if(!p)
                    return;

                auto ctrl= p->controller();
                m_connect= connect(ctrl, &vmap::VisualItemController::posEditFinished, m_lightCtrl,
                                   [this]() { m_lightCtrl->setScenePos(scenePos()); });
            });
    initChildPointItem();

    // Make sure Qt knows only the center area is interactive,
    // so cursor changes and hit-testing use shape() not boundingRect().
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setAcceptHoverEvents(true);
}

QRectF LightItem::boundingRect() const
{
    if(!m_lightCtrl)
        return {};
    // Full glow area must be in the bounding rect so it paints correctly,
    // but shape() below restricts the *interactive* region to the center dot.
    return computeBoundingRect(m_lightCtrl->radius());
}

QPainterPath LightItem::shape() const
{
    // Hit box = only the small center dot. Clicking anywhere in the glow
    // area will NOT select the item — only clicks on the dot will.
    QPainterPath path;
    path.addEllipse(QPointF(0, 0), CENTER_RADIUS, CENTER_RADIUS);
    return path;
}

void LightItem::setNewEnd(const QPointF& nend)
{
    if(m_lightCtrl)
        m_lightCtrl->setRadius(QLineF(m_lightCtrl->pos(), nend).length());
}

void LightItem::updateChildPosition()
{
    if(!m_lightCtrl)
        return;
    auto rect= boundingRect();
    m_children.value(0)->setPos(QPointF(rect.right(), rect.center().y()));
    m_children.value(0)->setPlacement(ChildPointItem::MiddleRight);
    update();
}

void LightItem::initChildPointItem()
{
    if(!m_lightCtrl)
        return;
    auto rect= computeBoundingRect(m_lightCtrl->radius());

    setTransformOriginPoint(rect.center());

    ChildPointItem* tmp= new ChildPointItem(m_ctrl, 0, this);
    m_children.append(tmp);

    m_children.value(0)->setPos(QPointF(rect.right(), rect.center().y()));
    m_children.value(0)->setPlacement(ChildPointItem::MiddleRight);
    m_children.value(0)->setMotion(ChildPointItem::X_AXIS);
}

void LightItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // Delegate to base class so dragging still works normally.
    VisualItem::mouseMoveEvent(event);
}

void LightItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if(!m_lightCtrl)
        return;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    setChildrenVisible(hasFocusOrChild());
    const qreal radius= m_lightCtrl->radius();
    const QPointF center(0, 0);

    // --- Radial glow ---
    QRadialGradient gradient(center, radius);
    gradient.setColorAt(0.0, QColor(255, 240, 150, 200)); // warm yellow core
    gradient.setColorAt(0.4, QColor(255, 200, 50, 100));  // softer mid
    gradient.setColorAt(1.0, QColor(255, 150, 0, 0));     // transparent edge

    painter->setPen(Qt::NoPen);
    painter->setBrush(gradient);
    painter->drawEllipse(center, radius, radius);

    // --- Center dot: changes color when selected ---
    const bool selected= isSelected();
    if(selected)
    {
        // Bright cyan ring + white fill to signal selection clearly
        painter->setBrush(QColor(255, 255, 255, 240));
        painter->setPen(QPen(QColor(0, 200, 255), 2.0));
    }
    else
    {
        painter->setBrush(QColor(255, 255, 200, 230));
        painter->setPen(Qt::NoPen);
    }
    painter->drawEllipse(center, CENTER_RADIUS, CENTER_RADIUS);

    painter->restore();
}
