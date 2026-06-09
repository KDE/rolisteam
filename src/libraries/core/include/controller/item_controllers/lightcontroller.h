
#ifndef LIGHTCONTROLLER_H
#define LIGHTCONTROLLER_H

#include <QColor>
#include <QPointF>
#include <QRectF>
#include <QVariant>

#include "visualitemcontroller.h"
#include <core_global.h>

class VectorialMapController;

namespace vmap
{
/**
 * @brief Controller for a light source item on the vectorial map.
 *
 * Stores position, radius, and color of a light.
 * The corresponding LightItem (view) uses this data to
 * draw the light glow and compute the visibility polygon
 * for fog-of-war interaction.
 */
class CORE_EXPORT LightController : public VisualItemController
{
    Q_OBJECT
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(QRectF rect READ rect NOTIFY rectChanged)

public:
    LightController(const std::map<QString, QVariant>& params,
                    VectorialMapController* ctrl,
                    QObject* parent = nullptr);

    qreal radius() const;
    QRectF rect() const override;

    void aboutToBeRemoved() override;
    void endGeometryChange() override;
    void setCorner(const QPointF& move, int corner,
                   Core::TransformType tt = Core::TransformType::NoTransform) override;

public slots:
    void setRadius(qreal radius);

signals:
    void radiusChanged();
    void rectChanged();

private:
    qreal m_radius = 200.0;
};

} // namespace vmap

#endif // LIGHTCONTROLLER_H