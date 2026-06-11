#ifndef SHADOWCASTER_H
#define SHADOWCASTER_H

#include <QLineF>
#include <QPointF>
#include <QPolygonF>
#include <QVector>
#include <core_global.h>

namespace vmap
{
/**
 * @brief Computes a visibility polygon from a light position
 * using raycasting against a set of wall segments.
 *
 * Algorithm:
 * 1. Collect endpoints of all wall segments
 * 2. Cast rays from light toward each endpoint (+ slight offsets)
 * 3. Find closest intersection per ray
 * 4. Sort by angle, connect into visibility polygon
 */
class CORE_EXPORT ShadowCaster
{
public:
    /**
     * @brief Compute visibility polygon
     * @param lightPos   Light position in scene coordinates
     * @param radius     Maximum light radius
     * @param segments   Wall segments in scene coordinates
     * @return           Visibility polygon (the lit area)
     */
    static QPolygonF computeVisibilityPolygon(
        const QPointF& lightPos,
        qreal radius,
        const QVector<QLineF>& segments);

private:
    struct RayHit
    {
        qreal angle;
        QPointF point;
    };

    static bool raySegmentIntersect(
        const QPointF& rayOrigin,
        const QPointF& rayDir,
        const QLineF& segment,
        qreal& tRay,
        QPointF& hitPoint);

    static QPointF castRay(
        const QPointF& origin,
        qreal angle,
        qreal radius,
        const QVector<QLineF>& segments);
};

} // namespace vmap
#endif // SHADOWCASTER_H