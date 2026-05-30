#include "imageitem.h"

#include <QBuffer>
#include <QDebug>
#include <QFileInfo>
#include <QImageWriter>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include "controller/item_controllers/imageitemcontroller.h"
#include "controller/item_controllers/visualitemcontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"

ImageItem::ImageItem(vmap::ImageItemController* ctrl) : VisualItem(ctrl), m_imgCtrl(ctrl)
{
    m_keepAspect= true;

    m_promoteTypeList << vmap::VisualItemController::ItemType::CHARACTER;

    for(int i= 0; i <= Core::BottomLeft; ++i)
    {
        ChildPointItem* tmp= new ChildPointItem(m_imgCtrl, i, this);
        tmp->setMotion(ChildPointItem::MOUSE);
        m_children.append(tmp);
    }

    connect(m_imgCtrl, &vmap::ImageItemController::rectChanged, this, [this]() { updateChildPosition(); });
    connect(m_imgCtrl, &vmap::ImageItemController::pixmapChanged, this, [this]() { update(); });
    if(m_imgCtrl)
        setTransformOriginPoint(m_imgCtrl->rect().center());
}

QRectF ImageItem::boundingRect() const
{
    return m_imgCtrl ? m_imgCtrl->rect() : QRectF();
}
void ImageItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget)
    painter->save();
    auto img= m_imgCtrl->pixmap();
    painter->drawPixmap(m_imgCtrl->rect(), img, img.rect());

    setChildrenVisible(hasFocusOrChild());

    painter->restore();

    if(canBeMoved() && (option->state & QStyle::State_MouseOver || isSelected()))
    {
        painter->save();
        QPen pen= painter->pen();
        pen.setColor(isSelected() ? m_selectedColor : m_highlightColor);
        pen.setWidth(m_highlightWidth);
        painter->setPen(pen);
        painter->drawRect(m_imgCtrl->rect());
        painter->restore();
    }
#ifdef QT_DEBUG
    paintCoord(painter);
#endif
}
void ImageItem::setNewEnd(const QPointF& p)
{
    m_imgCtrl->setCorner(p, Core::BottomRight);
}

void ImageItem::updateChildPosition()
{
    if(!m_imgCtrl)
        return;

    auto rect= m_imgCtrl->rect();

    if(!m_imgCtrl->networkUpdate())
    {
        auto oldScenePos= scenePos();
        setTransformOriginPoint(rect.center());
        auto newScenePos= scenePos();
        auto oldPos= pos();
        m_ctrl->setPos(QPointF(oldPos.x() + (oldScenePos.x() - newScenePos.x()),
                               oldPos.y() + (oldScenePos.y() - newScenePos.y())));
    }
    else // network update
    {
        updateScenePos();
    }

    m_children.value(0)->setPos(rect.topLeft());
    m_children.value(0)->setPlacement(ChildPointItem::TopLeft);
    m_children.value(1)->setPos(rect.topRight());
    m_children.value(1)->setPlacement(ChildPointItem::TopRight);
    m_children.value(2)->setPos(rect.bottomRight());
    m_children.value(2)->setPlacement(ChildPointItem::ButtomRight);
    m_children.value(3)->setPos(rect.bottomLeft());
    m_children.value(3)->setPlacement(ChildPointItem::ButtomLeft);

    update();
}

VisualItem* ImageItem::promoteTo(vmap::VisualItemController::ItemType type)
{
    Q_UNUSED(type)
    /*if(type == CHARACTER)
    {
        QFileInfo info(m_imagePath);
        Character* character= new Character(info.baseName(), Qt::black, true);
        character->setAvatar(m_image);
        CharacterItem* item= new CharacterItem(character, pos());
        //  item->set
        item->generatedThumbnail();
        item->setScale(scale());
        return item;
    }*/
    return nullptr;
}

QColor ImageItem::color(const QPointF& pos) const
{
    if(!m_imgCtrl)
        return {};
    auto rect= m_imgCtrl->rect();
    auto img= m_imgCtrl->pixmap().toImage();
    int x= pos.x() * img.width() / rect.width();
    int y= pos.y() * img.height() / rect.height();
    return img.pixelColor(QPoint{x, y});
}
