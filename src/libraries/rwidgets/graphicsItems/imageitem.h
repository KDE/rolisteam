#ifndef IMAGEITEM_H
#define IMAGEITEM_H

#include "rwidgets_global.h"
#include "visualitem.h"
#include <QImage>
#include <QMovie>
namespace vmap
{
class ImageItemController;
}
class RWIDGET_EXPORT ImageItem : public VisualItem
{
    Q_OBJECT
public:
    ImageItem(vmap::ImageItemController* ctrl);
    /**
     * @brief paint the current rectangle into the scene.
     * @see Qt documentation
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget= nullptr) override;

    /**
     * @brief gives bounding rect. Return rect geometry into the QRectF
     */
    virtual QRectF boundingRect() const override;

    /**
     * @brief defines new end point.
     */
    virtual void setNewEnd(const QPointF& nend) override;

    VisualItem* promoteTo(vmap::VisualItemController::ItemType) override;

    QColor color(const QPointF& pos) const override;

protected:
    /**
     * @brief updateChildPosition
     */
    virtual void updateChildPosition() override;

private:
    QPointer<vmap::ImageItemController> m_imgCtrl;
    bool m_keepAspect; ///< flag to keep the aspect.
};

#endif // IMAGEITEM_H
