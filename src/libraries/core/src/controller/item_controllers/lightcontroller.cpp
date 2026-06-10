
#include "controller/item_controllers/lightcontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include <cmath>
#include <QtMath>

namespace vmap
{

LightController::LightController(const std::map<QString, QVariant>& params,
                                 VectorialMapController* ctrl,
                                 QObject* parent)
    : VisualItemController(VisualItemController::LIGHT, params, ctrl, parent)
{
    auto it = params.find(QStringLiteral("radius"));
    if(it != params.end())
        m_radius = it->second.toReal();

    connect(this, &VisualItemController::posChanged, this, &LightController::updateFogReveal);
    connect(this, &LightController::radiusChanged,   this, &LightController::updateFogReveal);

    updateFogReveal();
}

qreal LightController::radius() const
{
    return m_radius;
}

QRectF LightController::rect() const
{
    return QRectF(-m_radius, -m_radius, m_radius * 2, m_radius * 2);
}

void LightController::setRadius(qreal radius)
{
    if(qFuzzyCompare(m_radius, radius))
        return;
    m_radius = radius;
    emit radiusChanged();
    emit rectChanged();
}

void LightController::aboutToBeRemoved()
{
    if(m_ctrl && m_ctrl->sightController())
        m_ctrl->sightController()->clearTempPolygons();
}

void LightController::endGeometryChange()
{
    VisualItemController::endGeometryChange();
}

void LightController::setCorner(const QPointF& move, int corner, Core::TransformType tt)
{
    Q_UNUSED(corner)
    Q_UNUSED(tt)
    setRadius(move.x());
}

void LightController::updateFogReveal()
{
    if(!m_ctrl) // m_ctrl is the VectorialMapController* in VisualItemController base
        return;

    auto* sightCtrl = m_ctrl->sightController();
    if(!sightCtrl)
        return;

    // Clear all previous temp reveals (from all lights)
    // then re-add this light's polygon
    sightCtrl->clearTempPolygons();

    // Build circle polygon (32-sided approximation is smooth enough)
    constexpr int segments = 32;
    QPolygonF circle;
    circle.reserve(segments);
    const QPointF center = pos(); // pos() from VisualItemController/QGraphicsObject
    for(int i = 0; i < segments; ++i)
    {
        double angle = 2.0 * M_PI * i / segments;
        circle << QPointF(center.x() + m_radius * std::cos(angle),
                          center.y() + m_radius * std::sin(angle));
    }

    // mask=false → subtract from fog (reveal), temp=true → dynamic, gets cleared on next update
    sightCtrl->addPolygon(circle, false, true);
}

} // namespace vmap