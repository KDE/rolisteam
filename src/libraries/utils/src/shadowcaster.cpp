#include <QPainterPath>
#include "utils/shadowcaster.h"
#include <QtMath>
#include <cmath>
#include <algorithm>

namespace ShadowCaster
{

// Precomputed boundary rays at ~5 degree intervals (constant, no need to recompute)
static QList<qreal> boundaryAngles()
{
    static QList<qreal> angles;
    if(angles.isEmpty())
    {
        constexpr int N = 72; // 360/5
        angles.reserve(N);
        for(int i = 0; i < N; ++i)
            angles << (2.0 * M_PI * i / N - M_PI);
    }
    return angles;
}

QPolygonF computeVisibilityPolygon(
    const QPointF& lightPos,
    qreal radius,
    const QList<QLineF>& segments)
{
    // Fallback: no walls — full circle using addEllipse approach
    if(segments.isEmpty())
    {
        QPainterPath path;
        path.addEllipse(lightPos, radius, radius);
        return path.toFillPolygon();
    }

    // Step 1: collect angles toward obstacle vertices only
    QList<qreal> angles;
    angles.reserve(segments.size() * 6);

    for(const QLineF& seg : segments)
    {
        for(const QPointF& pt : {seg.p1(), seg.p2()})
        {
            QPointF delta = pt - lightPos;
            if(delta.manhattanLength() > radius * 2)
                continue;
            qreal angle = std::atan2(delta.y(), delta.x());
            // 3 rays per vertex: peek around both sides of corner
            angles << angle - 0.00001;
            angles << angle;
            angles << angle + 0.00001;
        }
    }

    // Step 2: append precomputed boundary rays for open area coverage
    angles << boundaryAngles();

    // Step 3: sort and deduplicate
    std::sort(angles.begin(), angles.end());
    angles.erase(
        std::unique(angles.begin(), angles.end(),
            [](qreal a, qreal b) { return qAbs(a - b) < 0.000001; }),
        angles.end());

    // Step 4: cast each ray, collect hits
    struct RayHit { qreal angle; QPointF point; };
    QList<RayHit> hits;
    hits.reserve(angles.size());

    for(qreal angle : angles)
        hits.push_back({angle, castRay(lightPos, angle, radius, segments)});

    // Already sorted by angle since angles list was sorted

    // Step 5: build polygon
    QPolygonF poly;
    poly.reserve(hits.size() + 1);
    for(const RayHit& h : hits)
        poly << h.point;

    if(!poly.isEmpty())
        poly << poly.first();

    return poly;
}

QPointF castRay(
    const QPointF& origin,
    qreal angle,
    qreal radius,
    const QList<QLineF>& segments)
{
    QPointF dir(std::cos(angle), std::sin(angle));
    qreal closestT = radius;
    QPointF closestHit = origin + dir * radius;

    for(const QLineF& seg : segments)
    {
        qreal t = 0.0;
        QPointF hit;
        if(raySegmentIntersect(origin, dir, seg, t, hit))
        {
            if(t < closestT)
            {
                closestT = t;
                closestHit = hit;
            }
        }
    }

    return closestHit;
}

bool raySegmentIntersect(
    const QPointF& rayOrigin,
    const QPointF& rayDir,
    const QLineF& segment,
    qreal& tRay,
    QPointF& hitPoint)
{
    QPointF segDir = segment.p2() - segment.p1();
    QPointF originDelta = segment.p1() - rayOrigin;

    qreal cross = rayDir.x() * segDir.y() - rayDir.y() * segDir.x();

    if(qFuzzyIsNull(cross))
        return false;

    qreal t = (originDelta.x() * segDir.y() - originDelta.y() * segDir.x()) / cross;
    qreal u = (originDelta.x() * rayDir.y() - originDelta.y() * rayDir.x()) / cross;

    if(t >= 0.0 && u >= 0.0 && u <= 1.0)
    {
        tRay = t;
        hitPoint = rayOrigin + rayDir * t;
        return true;
    }

    return false;
}

} // namespace ShadowCaster