#include "imageitem.h"

#include <QBuffer>
#include <QDebug>
#include <QFileInfo>
#include <QImageWriter>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include "controller/view_controller/vectorialmapcontroller.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "vmap/controller/imagecontroller.h"
#include "vmap/controller/visualitemcontroller.h"

#include "characteritem.h"
#include "data/character.h"

ImageItem::ImageItem(vmap::ImageController* ctrl) : VisualItem(ctrl), m_imgCtrl(ctrl)
{
    m_keepAspect= true;

    m_promoteTypeList << vmap::VisualItemController::ItemType::CHARACTER;

    for(int i= 0; i <= vmap::ImageController::BottomLeft; ++i)
    {
        ChildPointItem* tmp= new ChildPointItem(m_imgCtrl, i, this);
        tmp->setMotion(ChildPointItem::MOUSE);
        m_children.append(tmp);
    }
    updateChildPosition();

    connect(m_imgCtrl, &vmap::ImageController::rectChanged, this, [this]() { updateChildPosition(); });
    setTransformOriginPoint(m_imgCtrl->rect().center());
}

QRectF ImageItem::boundingRect() const
{
    return m_imgCtrl->rect();
}
void ImageItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget)
    painter->save();
    auto img= m_imgCtrl->pixmap();
    painter->drawPixmap(m_imgCtrl->rect(), img, img.rect());

    setChildrenVisible(hasFocusOrChild());

    painter->restore();

    if(option->state & QStyle::State_MouseOver || isUnderMouse())
    {
        painter->save();
        QPen pen= painter->pen();
        pen.setColor(m_highlightColor);
        pen.setWidth(m_highlightWidth);
        painter->setPen(pen);
        painter->drawRect(m_imgCtrl->rect());
        painter->restore();
    }
}
void ImageItem::setNewEnd(const QPointF& p)
{
    m_imgCtrl->setCorner(p, vmap::ImageController::BottomRight);
}

void ImageItem::writeData(QDataStream& out) const
{
    // out << m_rect;
    /*  out << m_keepAspect;
      out << m_color;
      out << opacity();
      out << m_id;
      // out << static_cast<int>(m_layer);
      out << m_data;
      out << m_imagePath;
      out << m_initialized;
      out << m_ratio;
      out << scale();
      out << rotation();
      out << pos();*/
}

void ImageItem::readData(QDataStream& in)
{
    //   in >> m_rect;
    /*  in >> m_keepAspect;
      in >> m_color;
      qreal opa= 0;
      in >> opa;
      setOpacity(opa);
      in >> m_id;
      int i;
      in >> i;
      // m_layer= static_cast<Core::Layer>(i);
      in >> m_data;
      in >> m_imagePath;
      in >> m_initialized;
      in >> m_ratio;
      qreal scale;
      in >> scale;
      setScale(scale);

      qreal rotation;
      in >> rotation;
      setRotation(rotation);

      QPointF p;
      in >> p;
      setPos(p);

      dataToMedia();*/
}
void ImageItem::fillMessage(NetworkMessageWriter* msg)
{
    //  msg->string16(m_id);

    // rect
    /*    msg->real(m_rect.x());
        msg->real(m_rect.y());
        msg->real(m_rect.width());
        msg->real(m_rect.height());*/
    // msg->uint8(static_cast<quint8>(m_layer));
    /*   msg->real(zValue());
       msg->real(opacity());

       msg->int8(m_keepAspect);
       msg->rgb(m_color.rgb());

       //    QFile file(m_imagePath);
       //    if(file.exists())
       //    {
       //        file.open(QIODevice::ReadOnly);
       //        QByteArray baImage= file.readAll();
       //        msg->byteArray32(baImage);
       //    }
       //    else
       //    {
       //        QByteArray ba;
       //        QBuffer buffer(&ba);
       //        buffer.open(QIODevice::WriteOnly);
       //        m_image.save(&buffer, "PNG");
       //    }
       msg->byteArray32(m_data);

       msg->real(scale());
       msg->real(rotation());

       msg->real(pos().x());
       msg->real(pos().y());*/
}
void ImageItem::readItem(NetworkMessageReader* msg)
{
    // m_id= msg->string16();
    // rect
    /* m_rect.setX(msg->real());
     m_rect.setY(msg->real());
     m_rect.setWidth(msg->real());
     m_rect.setHeight(msg->real());*/
    // m_layer= static_cast<Core::Layer>(msg->int8());
    /* setZValue(msg->real());
      setOpacity(msg->real());

      m_keepAspect= msg->int8();
      m_color= msg->rgb();
      m_data= msg->byteArray32();

      dataToMedia();

      //  setTransformOriginPoint(m_rect.center());
      setScale(msg->real());
      setRotation(msg->real());

      qreal x= msg->real();
      qreal y= msg->real();
      setPos(x, y);*/
}
void ImageItem::setGeometryPoint(qreal pointId, QPointF& pos)
{
    /*  switch(static_cast<int>(pointId))
      {
      case 0:
          m_rect.setTopLeft(pos);
          m_child->value(1)->setPos(m_rect.topRight());
          m_child->value(2)->setPos(m_rect.bottomRight());
          m_child->value(3)->setPos(m_rect.bottomLeft());
          break;
      case 1:
          m_rect.setTopRight(pos);
          m_child->value(0)->setPos(m_rect.topLeft());
          m_child->value(2)->setPos(m_rect.bottomRight());
          m_child->value(3)->setPos(m_rect.bottomLeft());
          break;
      case 2:
          m_rect.setBottomRight(pos);
          m_child->value(0)->setPos(m_rect.topLeft());
          m_child->value(1)->setPos(m_rect.topRight());
          m_child->value(3)->setPos(m_rect.bottomLeft());
          break;
      case 3:
          m_rect.setBottomLeft(pos);
          m_child->value(0)->setPos(m_rect.topLeft());
          m_child->value(1)->setPos(m_rect.topRight());
          m_child->value(2)->setPos(m_rect.bottomRight());
          break;
      default:
          break;
      }

      setTransformOriginPoint(m_rect.center());*/

    // updateChildPosition();
}

void ImageItem::initChildPointItem()
{
    /*   if(!m_initialized)
       {
           // setPos(m_rect.center());
           m_rect.setCoords(-m_rect.width() / 2, -m_rect.height() / 2, m_rect.width() / 2, m_rect.height() / 2);
           m_initialized= true;
       }

       m_rect= m_rect.normalized();
       setTransformOriginPoint(m_rect.center());
       if((nullptr == m_child))
       {
           m_child= new QVector<ChildPointItem*>();
       }
       else
       {
           m_child->clear();
       }

       for(int i= 0; i < 4; ++i)
       {
           ChildPointItem* tmp= new ChildPointItem(m_ctrl, i, this);
           tmp->setMotion(ChildPointItem::MOUSE);
           m_child->append(tmp);
       }
       updateChildPosition();*/
}
void ImageItem::updateChildPosition()
{
    auto rect= m_imgCtrl->rect();
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

/*
void ImageItem::loadImage()
{
    QFile file(m_imagePath);
    if(!file.open(QIODevice::ReadOnly))
        return;
    m_data= file.readAll();
    dataToMedia();
}*/

void ImageItem::dataToMedia()
{
    /*  auto buf= new QBuffer(&m_data);
      buf->open(QIODevice::ReadOnly);

      m_movie= new QMovie(buf);
      if((m_movie->isValid()) && (m_movie->frameCount() > 1))
      {
          connect(m_movie, &QMovie::updated, this, &ImageItem::updateImageFromMovie);
          m_movie->start();
          // m_rect= m_movie->frameRect();
      }
      else
      {
          delete m_movie;
          m_movie= nullptr;
          m_image.loadFromData(m_data);

          initImage();
      }*/
}

void ImageItem::initImage()
{
    /* if(m_image.isNull())
         return;
     //  m_rect= m_image.rect();
     if(m_image.width() != 0)
     {
         m_ratio= m_image.height() / m_image.width();
     }
     QBuffer buffer;
     m_image.save(&buffer, "png");
     m_data= buffer.data();*/
}

void ImageItem::setModifiers(Qt::KeyboardModifiers modifiers)
{
    // m_modifiers= modifiers;
}
void ImageItem::updateImageFromMovie(QRect rect){
    Q_UNUSED(rect)
    //   if(nullptr != m_movie)
    //   {
    //      m_image= m_movie->currentImage();
    /*   if(m_rect.isNull())
       {
           m_rect= m_image.rect();
       }
       if(m_image.width() != 0)
       {
           m_ratio= m_image.height() / m_image.width();
       }*/
    //      update();
    // }
}

VisualItem* ImageItem::getItemCopy()
{
    /*ImageItem* rectItem= new Imagetem(m_ctrl);
    rectItem->setImageUri(m_imagePath);
    // rectItem->resizeContents(m_rect, VisualItem::NoTransform);
    rectItem->setPos(pos());*/
    //  return rectItem;
    return nullptr;
}

void ImageItem::endOfGeometryChange(ChildPointItem::Change change)
{
    if(change == ChildPointItem::Resizing)
    {
        auto oldScenePos= scenePos();
        setTransformOriginPoint(m_imgCtrl->rect().center());
        auto newScenePos= scenePos();
        auto oldPos= pos();
        m_imgCtrl->setPos(QPointF(oldPos.x() + (oldScenePos.x() - newScenePos.x()),
                                  oldPos.y() + (oldScenePos.y() - newScenePos.y())));
    }
    VisualItem::endOfGeometryChange(change);
}
VisualItem* ImageItem::promoteTo(vmap::VisualItemController::ItemType type)
{
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
