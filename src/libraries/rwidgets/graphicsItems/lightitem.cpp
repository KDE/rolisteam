
#include "lightitem.h"

#include <QPainter>
#include <QRadialGradient>
#include <QStyleOptionGraphicsItem>

#include "controller/item_controllers/lightcontroller.h"

LightItem::LightItem(vmap::LightController* ctrl) : VisualItem(ctrl), m_lightCtrl(ctrl)
{
    auto updateFunc= [this]() { updateChildPosition(); };
    connect(m_lightCtrl, &vmap::LightController::radiusChanged, this, updateFunc);
    initChildPointItem();
}

LightItem::~LightItem() {}

QRectF LightItem::boundingRect() const
{
    if(!m_lightCtrl)
        return {};
    return m_lightCtrl->rect();
}

void LightItem::setNewEnd(const QPointF& nend)
{
    if(m_lightCtrl)
        m_lightCtrl->setRadius(QLineF(m_lightCtrl->pos(), nend).length());
}

QPainterPath LightItem::shape() const
{
    QPainterPath path;
    if(m_lightCtrl)
        path.addEllipse(boundingRect());
    return path;
}

void LightItem::updateChildPosition()
{
    auto rect= boundingRect();
    m_children.value(0)->setPos(QPointF(rect.right(), rect.center().y()));
    m_children.value(0)->setPlacement(ChildPointItem::MiddleRight);
    update();
}

void LightItem::initChildPointItem()
{
    if(!m_lightCtrl)
        return;
    auto rect= boundingRect();

    setTransformOriginPoint(rect.center());

    ChildPointItem* tmp= new ChildPointItem(m_ctrl, 0, this);
    m_children.append(tmp);

    m_children.value(0)->setPos(QPointF(rect.right(), rect.center().y()));
    m_children.value(0)->setPlacement(ChildPointItem::MiddleRight);
    m_children.value(0)->setMotion(ChildPointItem::X_AXIS);
}

void LightItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if(!m_lightCtrl)
        return;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    qreal radius= m_lightCtrl->radius();
    QPointF center(0, 0); // item paints at local origin

    // Radial gradient: bright yellow center fading to transparent
    QRadialGradient gradient(center, radius);
    gradient.setColorAt(0.0, QColor(255, 240, 150, 200)); // warm yellow
    gradient.setColorAt(0.4, QColor(255, 200, 50, 100));  // softer
    gradient.setColorAt(1.0, QColor(255, 150, 0, 0));     // transparent edge

    painter->setPen(Qt::NoPen);
    painter->setBrush(gradient);
    painter->drawEllipse(center, radius, radius);

    // Small bright dot at center
    painter->setBrush(QColor(255, 255, 200, 230));
    painter->drawEllipse(center, 6, 6);

    painter->restore();
}