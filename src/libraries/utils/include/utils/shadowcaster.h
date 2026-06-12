#ifndef SHADOWCASTER_H
#define SHADOWCASTER_H

#include <QLineF>
#include <QList>
#include <QPointF>
#include <QPolygonF>
#include <utils_global.h>

/**
 * @brief ShadowCaster namespace provides raycasting-based visibility polygon computation.
 *
 * Algorithm:
 * 1. Cast rays from light toward each obstacle vertex
 * 2. Find closest intersection per ray against all wall segments
 * 3. Sort hit points by angle, connect into visibility polygon
 */
namespace ShadowCaster
{
    UTILS_EXPORT QPolygonF computeVisibilityPolygon(
        const QPointF& lightPos,
        qreal radius,
        const QList<QLineF>& segments);

    UTILS_EXPORT bool raySegmentIntersect(
        const QPointF& rayOrigin,
        const QPointF& rayDir,
        const QLineF& segment,
        qreal& tRay,
        QPointF& hitPoint);

    UTILS_EXPORT QPointF castRay(
        const QPointF& origin,
        qreal angle,
        qreal radius,
        const QList<QLineF>& segments);
}

#endif // SHADOWCASTER_H