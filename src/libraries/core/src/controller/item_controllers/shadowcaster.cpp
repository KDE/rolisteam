#include "controller/item_controllers/shadowcaster.h"
#include <QtMath>
#include <cmath>
#include <algorithm>
#include <limits>

namespace vmap
{

QPolygonF ShadowCaster::computeVisibilityPolygon(
    const QPointF& lightPos,
    qreal radius,
    const QVector<QLineF>& segments)
{
    if(segments.isEmpty())
{
    QPolygonF circle;
    constexpr int N = 64;
    for(int i = 0; i < N; ++i)
    {
        double a = 2.0 * M_PI * i / N;
        circle << QPointF(lightPos.x() + radius * std::cos(a),
                          lightPos.y() + radius * std::sin(a));
    }
    return circle;
}

    // Step 1: collect all unique angles to cast rays toward
    QVector<qreal> angles;
    angles.reserve(segments.size() * 4 + 64);

    for(const QLineF& seg : segments)
    {
        // Ray toward each endpoint of each segment
        for(const QPointF& pt : {seg.p1(), seg.p2()})
        {
            QPointF delta = pt - lightPos;
            if(delta.manhattanLength() > radius * 2)
                continue; // too far to matter
            qreal angle = std::atan2(delta.y(), delta.x());
            angles << angle - 0.00001;
            angles << angle;
            angles << angle + 0.00001;
        }
        // Deduplicate very similar angles
        std::sort(angles.begin(), angles.end());
        angles.erase(std::unique(angles.begin(), angles.end(),
            [](qreal a, qreal b) { return qAbs(a - b) < 0.000001; }),
            angles.end());
    }

    // Step 2: add boundary rays every ~5 degrees so open areas get coverage
    constexpr int boundaryRays = 72; // 360/5
    for(int i = 0; i < boundaryRays; ++i)
        angles << (2.0 * M_PI * i / boundaryRays - M_PI);

    // Step 3: cast each ray, find closest hit
    QVector<RayHit> hits;
    hits.reserve(angles.size());

    for(qreal angle : angles)
    {
        QPointF hit = castRay(lightPos, angle, radius, segments);
        hits.push_back({angle, hit});
    }

    // Step 4: sort by angle
    std::sort(hits.begin(), hits.end(),
              [](const RayHit& a, const RayHit& b) { return a.angle < b.angle; });

    // Step 5: build polygon
    QPolygonF poly;
    poly.reserve(hits.size() + 1);
    for(const RayHit& h : hits)
        poly << h.point;

    if(!poly.isEmpty())
        poly << poly.first(); // close it

    return poly;
}

QPointF ShadowCaster::castRay(
    const QPointF& origin,
    qreal angle,
    qreal radius,
    const QVector<QLineF>& segments)
{
    QPointF dir(std::cos(angle), std::sin(angle));

    qreal closestT = radius; // default: hits radius boundary
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

bool ShadowCaster::raySegmentIntersect(
    const QPointF& rayOrigin,
    const QPointF& rayDir,
    const QLineF& segment,
    qreal& tRay,
    QPointF& hitPoint)
{
    // Ray: P = rayOrigin + t * rayDir
    // Seg: Q = seg.p1() + u * (seg.p2() - seg.p1())
    // Solve for t and u

    QPointF segDir = segment.p2() - segment.p1();
    QPointF originDelta = segment.p1() - rayOrigin;

    qreal cross = rayDir.x() * segDir.y() - rayDir.y() * segDir.x();

    if(qFuzzyIsNull(cross))
        return false; // parallel

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

} // namespace vmap