
#include "controller/item_controllers/lightcontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"

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
}

qreal LightController::radius() const
{
    return m_radius;
}

QRectF LightController::rect() const
{
    return QRectF(pos().x() - m_radius,
                  pos().y() - m_radius,
                  m_radius * 2,
                  m_radius * 2);
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

} // namespace vmap