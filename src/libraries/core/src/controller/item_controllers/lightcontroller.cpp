#include "controller/item_controllers/pathcontroller.h"
#include "controller/item_controllers/lightcontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "model/vmapitemmodel.h"
#include "utils/shadowcaster.h"
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

    connect(this, &VisualItemController::posChanged,   this, &LightController::updateFogReveal);
    connect(this, &LightController::radiusChanged,     this, &LightController::updateFogReveal);

    if(m_ctrl && m_ctrl->model())
    {
        connect(m_ctrl->model(), &vmap::VmapItemModel::itemControllerAdded,
                this, [this](vmap::VisualItemController* item) {
                    if(!item)
                        return;
                    // Update when item finishes initializing (geometry is final)
                    connect(item, &vmap::VisualItemController::initializedChanged,
                            this, &LightController::updateFogReveal,
                            Qt::UniqueConnection);
                    // For path items, also update when points are added
                    if(auto* path = dynamic_cast<vmap::PathController*>(item))
                    {
                        connect(path, &vmap::PathController::pointCountChanged,
                                this, &LightController::updateFogReveal,
                                Qt::UniqueConnection);
                    }
                });
        connect(m_ctrl->model(), &vmap::VmapItemModel::itemControllersRemoved,
                this, &LightController::updateFogReveal);
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

    QList<QLineF> segments = collectWallSegments();
    QPolygonF visibilityPoly = ShadowCaster::computeVisibilityPolygon(pos(), m_radius, segments);

    if(segments.isEmpty())
    {
        // No walls — just reveal the full circle
        sightCtrl->addPolygon(visibilityPoly, false, true);
        return;
    }

    // Use full circle minus visibility polygon to get smooth shadow edge:
    // The circle gives smooth outer edge, visibility polygon cuts out the lit area
    QPainterPath circlePath;
    circlePath.addEllipse(pos(), m_radius, m_radius);

    QPainterPath visPath;
    visPath.addPolygon(visibilityPoly);

    // Reveal = intersection of circle and visibility polygon
    QPainterPath revealPath = circlePath.intersected(visPath);
    sightCtrl->addPolygon(revealPath.toFillPolygon(), false, true);
}

QList<QLineF> LightController::collectWallSegments() const
{
    QList<QLineF> segments;
    if(!m_ctrl || !m_ctrl->model())
        return segments;

    static QSet<vmap::VisualItemController::ItemType> accepted{
        vmap::VisualItemController::ELLIPSE,
        vmap::VisualItemController::LINE,
        vmap::VisualItemController::RECT,
        vmap::VisualItemController::PATH
    };

    for(auto* item : m_ctrl->model()->items())
    {
        if(!item || item->removed())
            continue;

        auto layer = item->layer();
        if(layer != Core::Layer::GROUND && layer != Core::Layer::OBJECT)
            continue;

        if(!accepted.contains(item->itemType()))
            continue;

        QPolygonF poly = item->obstaclePolygon();

        for(int i = 0; i + 1 < poly.size(); ++i)
            segments << QLineF(poly[i], poly[i + 1]);
    }

    return segments;
}

} // namespace vmap