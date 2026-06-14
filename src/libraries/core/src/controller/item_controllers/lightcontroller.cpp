#include "controller/item_controllers/lightcontroller.h"
#include "controller/item_controllers/pathcontroller.h"
#include "controller/item_controllers/rectcontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "model/vmapitemmodel.h"
#include "utils/shadowcaster.h"
#include "worker/utilshelper.h"
#include <QPainterPath>

namespace vmap
{

LightController::LightController(const std::map<QString, QVariant>& params, VectorialMapController* ctrl,
                                 QObject* parent)
    : VisualItemController(VisualItemController::LIGHT, params, ctrl, parent)
{
    namespace hu= helper::utils;
    namespace cv= Core::vmapkeys;
    using std::placeholders::_1;
    hu::setParamIfAny<qreal>(cv::KEY_RADIUS, params, std::bind(&LightController::setRadius, this, _1));

    auto updateFog= [this]() { updateFogReveal(); }; // to use the default parameter

    connect(this, &VisualItemController::posChanged, this, updateFog);
    connect(this, &LightController::radiusChanged, this, updateFog);
    connect(this, &VisualItemController::initializedChanged, this, [this]() { updateFogReveal(false); });

    if(m_ctrl && m_ctrl->model())
    {

        connect(m_ctrl->model(), &vmap::VmapItemModel::itemControllerAdded, this,
                [this, updateFog](vmap::VisualItemController* item)
                {
                    if(!item)
                        return;

                    connect(item, &vmap::VisualItemController::initializedChanged, this, updateFog);
                    // Update shadow when obstacle is rotated
                    connect(item, &vmap::VisualItemController::rotationChanged, this, updateFog);
                    if(auto* path= dynamic_cast<vmap::PathController*>(item))
                    {
                        connect(path, &vmap::PathController::pointCountChanged, this, updateFog);
                    }
                });
        connect(m_ctrl->model(), &vmap::VmapItemModel::itemControllersRemoved, this, updateFog);
    }

    connect(this, &LightController::visibleChanged, this, [this]() { aboutToBeRemoved(); });

    updateFogReveal(false);
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
    m_radius= radius;
    emit radiusChanged();
}

void LightController::aboutToBeRemoved()
{
    if(m_ctrl && m_ctrl->sightController())
    {
        m_ctrl->sightController()->setBlockU(false);
        m_ctrl->sightController()->clearTempPolygons();
    }
}

void LightController::endGeometryChange()
{
    if(m_ctrl && m_ctrl->sightController())
        m_ctrl->sightController()->setBlockU(false);
    VisualItemController::endGeometryChange();
}

void LightController::setCorner(const QPointF& move, int corner, Core::TransformType tt)
{
    Q_UNUSED(corner)
    Q_UNUSED(tt)
    setRadius(std::max(radius() + move.x(), 10.));
}

void LightController::updateFogReveal(bool blockUpdate)
{
    if(!m_ctrl)
        return;

    auto* sightCtrl= m_ctrl->sightController();
    if(!sightCtrl)
        return;

    sightCtrl->setBlockU(blockUpdate);
    sightCtrl->clearTempPolygons();

    QList<QLineF> segments= collectWallSegments();
    QPolygonF visibilityPoly= ShadowCaster::computeVisibilityPolygon(pos(), m_radius, segments);

    if(segments.isEmpty())
    {
        sightCtrl->addPolygon(visibilityPoly, false, true);
        return;
    }

    QPainterPath circlePath;
    circlePath.setFillRule(Qt::WindingFill);
    circlePath.addEllipse(pos(), m_radius, m_radius);

    QPainterPath visPath;
    visPath.setFillRule(Qt::WindingFill);
    visPath.addPolygon(visibilityPoly);

    QPainterPath revealPath= circlePath.intersected(visPath);
    sightCtrl->addPolygon(revealPath.toFillPolygon(), false, true);
}

QList<QLineF> LightController::collectWallSegments() const
{
    QList<QLineF> segments;
    if(!m_ctrl || !m_ctrl->model())
        return segments;

    static QSet<vmap::VisualItemController::ItemType> accepted{
        vmap::VisualItemController::ELLIPSE, vmap::VisualItemController::LINE, vmap::VisualItemController::RECT,
        vmap::VisualItemController::PATH};

    for(auto* item : m_ctrl->model()->items())
    {
        if(!item || item->removed())
            continue;

        auto layer= item->layer();
        if(layer != Core::Layer::GROUND && layer != Core::Layer::OBJECT)
            continue;

        if(!accepted.contains(item->itemType()))
            continue;

        QPolygonF poly= item->obstaclePolygon();

        // Determine rotation center per item type:
        // RectItem rotates around rect().center() (setTransformOriginPoint)
        // all other items rotate around local origin (0,0)
        QPointF rotationCenter(0, 0);
        if(item->itemType() == VisualItemController::RECT)
        {
            auto* rectCtrl= dynamic_cast<RectController*>(item);
            if(rectCtrl)
                rotationCenter= rectCtrl->rect().center();
        }

        QTransform transform;
        transform.translate(item->pos().x(), item->pos().y());
        transform.translate(rotationCenter.x(), rotationCenter.y());
        transform.rotate(item->rotation());
        transform.translate(-rotationCenter.x(), -rotationCenter.y());
        poly= transform.map(poly);

        for(int i= 0; i + 1 < poly.size(); ++i)
            segments << QLineF(poly[i], poly[i + 1]);
    }

    return segments;
}

} // namespace vmap
