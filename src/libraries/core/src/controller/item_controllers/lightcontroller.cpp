
#include "controller/item_controllers/lightcontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "controller/item_controllers/linecontroller.h"
#include "controller/item_controllers/rectcontroller.h"
#include "controller/item_controllers/pathcontroller.h"
#include "controller/item_controllers/ellipsecontroller.h"
#include "model/vmapitemmodel.h"
#include <QTimer> 
#include "controller/item_controllers/shadowcaster.h"
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

    // Recalculate when items are added or removed from the map
    if(m_ctrl && m_ctrl->model())
{
    connect(m_ctrl->model(), &vmap::VmapItemModel::itemControllerAdded,
        this, [this](vmap::VisualItemController* item) {
            // Update immediately and also when the new item finishes being placed
            QTimer::singleShot(100, this, &LightController::updateFogReveal);
            if(item)
                connect(item, &vmap::VisualItemController::posEditFinished,
                        this, &LightController::updateFogReveal,
                        Qt::UniqueConnection);
        });
    connect(m_ctrl->model(), &vmap::VmapItemModel::itemControllersRemoved,
            this, [this](const QStringList&) {
                QTimer::singleShot(100, this, &LightController::updateFogReveal);
            });
}

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

    QVector<QLineF> segments = collectWallSegments();
    QPolygonF visibilityPoly = ShadowCaster::computeVisibilityPolygon(pos(), m_radius, segments);

    sightCtrl->addPolygon(visibilityPoly, false, true);
}

QVector<QLineF> LightController::collectWallSegments() const
{
    QVector<QLineF> segments;
    if(!m_ctrl || !m_ctrl->model())
        return segments;

    for(auto* item : m_ctrl->model()->items())
    {
        if(!item || item->removed())
            continue;

        // Only cast shadows from GROUND and OBJECT layers
        auto layer = item->layer();
        if(layer != Core::Layer::GROUND && layer != Core::Layer::OBJECT)
            continue;

        QPointF offset = item->pos();

        switch(item->itemType())
        {
        case VisualItemController::LINE:
        {
            auto* line = static_cast<LineController*>(item);
            segments << QLineF(offset + line->startPoint(),
                               offset + line->endPoint());
            break;
        }
        case VisualItemController::RECT:
        {
            auto* rect = static_cast<RectController*>(item);
            QRectF r = rect->rect().translated(offset);
            segments << QLineF(r.topLeft(),     r.topRight());
            segments << QLineF(r.topRight(),    r.bottomRight());
            segments << QLineF(r.bottomRight(), r.bottomLeft());
            segments << QLineF(r.bottomLeft(),  r.topLeft());
            break;
        }
        case VisualItemController::PATH:
        {
            auto* path = static_cast<PathController*>(item);
            const auto& pts = path->points();
            for(size_t i = 0; i + 1 < pts.size(); ++i)
                segments << QLineF(offset + pts[i], offset + pts[i+1]);
            if(path->closed() && pts.size() > 1)
                segments << QLineF(offset + pts.back(), offset + pts.front());
            break;
        }
        case VisualItemController::ELLIPSE:
        {
            auto* ellipse = static_cast<EllipseController*>(item);
            QPointF center = offset + ellipse->rect().center();
            qreal rx = ellipse->rx();
            qreal ry = ellipse->ry();
            // Approximate ellipse as 32-segment polygon of line segments
            constexpr int N = 32;
            for(int i = 0; i < N; ++i)
            {
                double a1 = 2.0 * M_PI * i / N;
                double a2 = 2.0 * M_PI * (i + 1) / N;
                QPointF p1(center.x() + rx * std::cos(a1),
                        center.y() + ry * std::sin(a1));
                QPointF p2(center.x() + rx * std::cos(a2),
                        center.y() + ry * std::sin(a2));
                segments << QLineF(p1, p2);
            }
            break;
        }
        default:
            break;
        }
    }
    return segments;
}

} // namespace vmap