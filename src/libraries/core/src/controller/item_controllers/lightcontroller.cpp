
#include "controller/item_controllers/lightcontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include <cmath>
#include <QtMath>
#include <QPainterPath>

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
    if(!m_ctrl)
        return;

    auto* sightCtrl = m_ctrl->sightController();
    if(!sightCtrl)
        return;

    sightCtrl->clearTempPolygons();

    QPainterPath path;
    path.addEllipse(pos(), m_radius, m_radius);
    sightCtrl->addPolygon(path.toFillPolygon(), false, true);
}

} // namespace vmap