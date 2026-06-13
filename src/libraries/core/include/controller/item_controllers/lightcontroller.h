#ifndef LIGHTCONTROLLER_H
#define LIGHTCONTROLLER_H

#include <QLineF>
#include <QRectF>
#include <QVariant>

#include "visualitemcontroller.h"
#include <core_global.h>

class VectorialMapController;

namespace vmap
{
/**
 * @brief Controller for a light source item on the vectorial map.
 */
class CORE_EXPORT LightController : public VisualItemController
{
    Q_OBJECT
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(QRectF rect READ rect NOTIFY radiusChanged)

public:
    LightController(const std::map<QString, QVariant>& params, VectorialMapController* ctrl, QObject* parent= nullptr);

    qreal radius() const;
    QRectF rect() const override;
    void aboutToBeRemoved() override;
    void endGeometryChange() override;
    void setCorner(const QPointF& move, int corner, Core::TransformType tt= Core::TransformType::NoTransform) override;

public slots:
    void setRadius(qreal radius);

signals:
    void radiusChanged();

private:
    void updateFogReveal();
    QList<QLineF> collectWallSegments() const;
    qreal m_radius = 200.0;
};

} // namespace vmap
#endif // LIGHTCONTROLLER_H
