
#ifndef LIGHTITEM_H
#define LIGHTITEM_H

#include "rwidgets_global.h"
#include "visualitem.h"
#include <QPointer>

namespace vmap
{
class LightController;
}

/**
 * @brief Renders a light source on the vectorial map.
 *
 * Draws a radial glow at the light's position.
 * Will eventually interact with SightController
 * to dynamically reveal fog of war.
 */
class RWIDGET_EXPORT LightItem : public VisualItem
{
    Q_OBJECT
public:
    explicit LightItem(vmap::LightController* ctrl);
    virtual ~LightItem();

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    QRectF boundingRect() const override;
    virtual void setNewEnd(const QPointF& nend) override;
    virtual QPainterPath shape() const override;

protected:
    virtual void updateChildPosition() override;
    void initChildPointItem();
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

private:
    QPointer<vmap::LightController> m_lightCtrl;
};

#endif // LIGHTITEM_H