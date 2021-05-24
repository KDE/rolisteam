/***************************************************************************
 *      Copyright (C) 2010 by Renaud Guezennec                             *
 *                                                                         *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "characteritem.h"

#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMenu>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include "data/character.h"
#include "data/player.h"
#include "dicealias.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "userlist/playermodel.h"

#include "controller/view_controller/vectorialmapcontroller.h"
#include "preferences/preferencesmanager.h"
#include "vmap/controller/characteritemcontroller.h"
#include "vmap/controller/visualitemcontroller.h"
#include "vmap/vmap.h"

#define MARGING 1
#define MINI_VALUE 25
#define RADIUS_CORNER 10
#define MAX_CORNER_ITEM 6
#define DIRECTION_RADIUS_HANDLE 4
#define ANGLE_HANDLE 5

void updateListAlias(QList<DiceAlias*>& list)
{
    /*auto preferences= PreferencesManager::getInstance();
    list.clear();
    int size= preferences->value("DiceAliasNumber", 0).toInt();
    for(int i= 0; i < size; ++i)
    {
        QString cmd= preferences->value(QString("DiceAlias_%1_command").arg(i), "").toString();
        QString value= preferences->value(QString("DiceAlias_%1_value").arg(i), "").toString();
        bool replace= preferences->value(QString("DiceAlias_%1_type").arg(i), true).toBool();
        bool enable= preferences->value(QString("DiceAlias_%1_enable").arg(i), true).toBool();
        //list.append(new DiceAlias(cmd, value, replace, enable));
    }*/
}

QRect makeSquare(QRect rect)
{
    if(rect.width() < rect.height())
        rect.setWidth(rect.height());
    else
        rect.setHeight(rect.width());

    return rect;
}

QColor ContrastColor(QColor color)
{
    int d= 0;

    // Counting the perceptive luminance - human eye favors green color...
    double luminance= (0.299 * color.red() + 0.587 * color.green() + 0.114 * color.blue()) / 255;

    if(luminance > 0.5)
        d= 0; // bright colors - black font
    else
        d= 255; // dark colors - white font

    return QColor(d, d, d);
}

//////////////////
// code of CharacterItem class.
/////////////////
CharacterItem::CharacterItem(vmap::CharacterItemController* ctrl) : VisualItem(ctrl), m_itemCtrl(ctrl)
{
    m_itemCtrl->setFont(QFont());

    connect(m_itemCtrl, &vmap::CharacterItemController::thumnailRectChanged, this, &CharacterItem::updateChildPosition);
    // createActions();
    for(int i= 0; i <= CharacterItem::SightLenght; ++i)
    {
        ChildPointItem* tmp= new ChildPointItem(m_itemCtrl, i, this);
        tmp->setMotion(ChildPointItem::MOUSE);
        m_children.append(tmp);
    }
    updateChildPosition();
}

CharacterItem::~CharacterItem()
{
    qDebug() << "charcteritem destroyed";
}

/*CharacterItem::CharacterItem(CharacterController* ctrl, Character* m, const QPointF& pos, qreal diameter)
    : VisualItem(ctrl)
    , m_center(pos)
    , m_diameter(diameter)
    , m_thumnails(nullptr)
    , m_protectGeometryChange(false)
    , m_visionChanged(false)
{
    setCharacter(m);
    setPos(m_center - QPointF(diameter / 2, diameter / 2));
    sizeChanged(diameter);
    createActions();
}*/

void CharacterItem::writeData(QDataStream& out) const
{
    /* out << m_center;
     int diam= static_cast<int>(m_diameter);
     out << diam;
     out << *m_thumnails;
     // out << m_rect;
     out << opacity();
     // out << static_cast<int>(m_layer);
     // out << zValue();
     if(nullptr != m_character)
     {
         out << true;
         m_character->writeData(out);
     }
     else
     {
         out << false;
     }*/
}

void CharacterItem::readData(QDataStream& in)
{
    /*    in >> m_center;
        int diam;
        in >> diam;
        m_diameter= diam;
        m_thumnails= new QPixmap();
        in >> *m_thumnails;
        // in >> m_rect;

        qreal opa= 0;
        in >> opa;
        setOpacity(opa);

        int tmp;
        in >> tmp;
        // m_layer= static_cast<Core::Layer>(tmp);
        bool hasCharacter;
        in >> hasCharacter;
        auto charact= new Character();
        if(hasCharacter)
        {
            charact->readData(in);
        }
        setCharacter(charact);*/
}

QRectF CharacterItem::boundingRect() const
{
    return m_itemCtrl->thumnailRect().united(m_itemCtrl->textRect());
}
QPainterPath CharacterItem::shape() const
{
    return m_itemCtrl->shape();
}

void CharacterItem::setNewEnd(const QPointF& nend)
{
    Q_UNUSED(nend);
    // m_center = nend;
}

void CharacterItem::setChildrenVisible(bool b)
{
    VisualItem::setChildrenVisible(b);

    /* if(m_child && (m_ctrl->permission() == Core::PC_MOVE || isNpc()))
     {
         if(!m_ctrl->localGM() || isNpc())
         {
             if(!m_child->isEmpty() && m_child->size() > DIRECTION_RADIUS_HANDLE)
             {
                 m_child->at(DIRECTION_RADIUS_HANDLE)->setVisible(false);
             }
             if(!m_child->isEmpty() && m_child->size() > ANGLE_HANDLE)
             {
                 m_child->at(ANGLE_HANDLE)->setVisible(false);
             }
         }
     }*/
}
#define PEN_WIDTH 6
#define PEN_RADIUS 3
void CharacterItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    bool hasFocusOrChildren= hasFocusOrChild();
    setChildrenVisible(hasFocusOrChildren);
    emit selectStateChange(hasFocusOrChildren);

    QString textToShow= m_itemCtrl->text();

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

    painter->save();
    if(m_itemCtrl->hasAvatar())
    {
        painter->drawImage(m_itemCtrl->thumnailRect(), *m_itemCtrl->avatar(), m_itemCtrl->avatar()->rect());
    }
    else
    {
        painter->setPen(m_itemCtrl->color());
        painter->setBrush(QBrush(m_itemCtrl->color(), Qt::SolidPattern));
        painter->drawEllipse(m_itemCtrl->thumnailRect());
    }

    QPen pen= painter->pen();
    pen.setWidth(PEN_WIDTH);
    auto character= m_itemCtrl->character();
    if(nullptr != character)
    {
        auto stateImg= m_itemCtrl->stateImage();
        if(!stateImg.isNull())
        {
            painter->drawImage(m_itemCtrl->thumnailRect(), stateImg, stateImg.rect());
        }
        else if(!character->hasAvatar())
        {
            pen.setColor(m_itemCtrl->stateColor());
            painter->setPen(pen);
            painter->drawEllipse(m_itemCtrl->thumnailRect().adjusted(PEN_RADIUS, PEN_RADIUS, -PEN_RADIUS, -PEN_RADIUS));
        }
        else
        {
            pen.setWidth(PEN_WIDTH / 2);
            pen.setColor(m_itemCtrl->stateColor());
            painter->setPen(pen);
            int diam= static_cast<int>(m_itemCtrl->side());
            painter->drawRoundedRect(0, 0, diam, diam, m_itemCtrl->side() / RADIUS_CORNER,
                                     m_itemCtrl->side() / RADIUS_CORNER);
        }
    }
    // QRectF rectText;
    QFontMetrics metric(painter->font());

    if(!textToShow.isEmpty())
    {
        setToolTip(textToShow);
        painter->setPen(m_itemCtrl->color());
        painter->drawText(m_itemCtrl->textRect(), Qt::AlignCenter, textToShow);
    }
    painter->restore();

    if(option->state & QStyle::State_MouseOver || isUnderMouse())
    {
        painter->save();
        QPen pen= painter->pen();
        pen.setColor(m_highlightColor);
        pen.setWidth(m_highlightWidth);
        painter->setPen(pen);
        if(m_itemCtrl->hasAvatar())
            painter->drawRect(m_itemCtrl->thumnailRect());
        else
            painter->drawEllipse(m_itemCtrl->thumnailRect());
        painter->restore();
    }

    if(m_itemCtrl->healthStatusVisible())
    {
        auto character= m_itemCtrl->character();
        if(nullptr == character)
            return;

        auto max= character->getHealthPointsMax();
        auto color= character->getLifeColor();
        auto min= character->getHealthPointsMin();
        auto current= character->getHealthPointsCurrent();
        QPen pen= painter->pen();
        pen.setColor(color);

        if(min < max)
        {
            auto rect= m_itemCtrl->thumnailRect();
            QRectF bar(rect.x(), rect.height() - PEN_WIDTH, rect.width(), PEN_WIDTH);
            painter->save();
            auto newWidth= (current - min) * bar.width() / (max - min);
            painter->drawRect(bar);
            QRectF value(rect.x(), rect.height() - PEN_WIDTH, newWidth, PEN_WIDTH);
            painter->fillRect(value, color);
            painter->restore();
        }
    }

    /// debug collision
    /*painter->save();
    QPen pen2= painter->pen();
    pen2.setColor(Qt::red);
    pen2.setWidth(1);
    painter->setPen(pen2);
    // painter->setBrush(QBrush(Qt::red, Qt::SolidPattern));

    QPainterPath path;
    path.addEllipse(shape().boundingRect().center(), 10, 10);
    path.connectPath(shape().translated(m_newPosition - pos()));
    painter->drawPath(path);

    pen2.setColor(Qt::green);
    pen2.setWidth(1);
    painter->setBrush(QBrush(Qt::green, Qt::SolidPattern));
    painter->setPen(pen2);

    painter->drawEllipse(shape().boundingRect().center(), 10, 10);
    painter->restore();
    */
}

void CharacterItem::sizeChanged(qreal m_size)
{
    // m_diameter= m_size;
    // m_rect.setRect(0, 0, m_diameter, m_diameter);
    // generatedThumbnail();
    // m_resizing= true;
}
void CharacterItem::visionChanged()
{
    m_visionChanged= true;
}
void CharacterItem::setSize(QSizeF size)
{
    m_protectGeometryChange= true;
    sizeChanged(size.width());
    updateChildPosition();
    m_protectGeometryChange= false;
    update();
}
void CharacterItem::setRectSize(qreal x, qreal y, qreal w, qreal h)
{
    VisualItem::setRectSize(x, y, w, h);
    // m_diameter= m_rect.width();
    generatedThumbnail();
    updateChildPosition();
}
void CharacterItem::generatedThumbnail()
{
    /* if(m_thumnails != nullptr)
     {
         delete m_thumnails;
         m_thumnails= nullptr;
     }
     int diam= static_cast<int>(m_diameter);
     m_thumnails= new QPixmap(diam, diam);
     m_thumnails->fill(Qt::transparent);
     QPainter painter(m_thumnails);
     QBrush brush;
     if(m_character->getAvatar().isNull())
     {
         painter.setPen(m_character->getColor());
         brush.setColor(m_character->getColor());
         brush.setStyle(Qt::SolidPattern);
     }
     else
     {
         painter.setPen(Qt::NoPen);
         QImage img= m_character->getAvatar();
         brush.setTextureImage(img.scaled(diam, diam));
     }

     painter.setBrush(brush);
     painter.drawRoundedRect(0, 0, diam, diam, m_diameter / RADIUS_CORNER, m_diameter / RADIUS_CORNER);*/
}

void CharacterItem::fillMessage(NetworkMessageWriter* msg)
{
    /*  if(nullptr == m_character || nullptr == m_vision || nullptr == msg)
          return;

      msg->string16(m_id);
      msg->real(scale());
      msg->real(rotation());
      msg->string16(m_character->getUuid());
      msg->real(m_diameter);

      // msg->uint8(static_cast<quint8>(m_layer));
      msg->real(zValue());
      msg->real(opacity());

      // pos
      msg->real(pos().x());
      msg->real(pos().y());

      msg->real(m_center.x());
      msg->real(m_center.y());

      // rect
      / * msg->real(m_rect.x());
       msg->real(m_rect.y());
       msg->real(m_rect.width());
       msg->real(m_rect.height());* /

      // path
      QByteArray data;
      QDataStream in(&data, QIODevice::WriteOnly);
      in.setVersion(QDataStream::Qt_5_7);
      if((m_thumnails == nullptr) || (m_thumnails->isNull()))
      {
          generatedThumbnail();
      }
      in << *m_thumnails;
      msg->byteArray32(data);

      m_character->fill(*msg, true);
      m_vision->fill(msg);*/
}
void CharacterItem::readItem(NetworkMessageReader* msg)
{
    /*  m_id= msg->string16();
      setScale(msg->real());
      setRotation(msg->real());
      QString idCharacter= msg->string16();
      m_diameter= msg->real();

      // m_layer= static_cast<Core::Layer>(msg->uint8());

      setZValue(msg->real());
      setOpacity(msg->real());

      qreal x= msg->real();
      qreal y= msg->real();

      setPos(x, y);
      // pos
      m_center.setX(msg->real());
      m_center.setY(msg->real());
      // rect

      / *  m_rect.setX(msg->real());
        m_rect.setY(msg->real());
        m_rect.setWidth(msg->real());
        m_rect.setHeight(msg->real());* /

      // path
      QByteArray data;
      data= msg->byteArray32();

      QDataStream out(&data, QIODevice::ReadOnly);
      out.setVersion(QDataStream::Qt_5_7);
      m_thumnails= new QPixmap();
      out >> *m_thumnails;

      / *Character* tmp= PlayerModel::instance()->getCharacter(idCharacter);

      if(nullptr != tmp)
      {
          tmp->read(*msg);
      }
      else
      {
          /// @todo This code may no longer be needed.
          tmp= new Character();
          QString id= tmp->read(*msg);
          tmp->setParentPerson(PlayerModel::instance()->getPlayer(id));
      }
      setCharacter(tmp);* /
      generatedThumbnail();

      updateItemFlags();

      if(nullptr != m_vision)
      {
          m_vision->readMessage(msg);
      }*/
}
void CharacterItem::resizeContents(const QRectF& rect, int pointId, TransformType)
{
    /*if(!rect.isValid())
        return;

    prepareGeometryChange();
    / * m_rect= rect;
     m_diameter= qMin(m_rect.width(), m_rect.height());* /
    sizeChanged(m_diameter);
    updateChildPosition();*/
}
QString CharacterItem::getCharacterId() const
{
    /*  if(nullptr != m_character)
      {
          return m_character->getUuid();
      }*/
    return QString();
}
void CharacterItem::setNumber(int n)
{
    /* if(nullptr == m_character)
         return;

     m_character->setNumber(n);*/
}
QString CharacterItem::getName() const
{
    /* if(nullptr == m_character)
         return {};

     return m_character->name();*/
    return {};
}
int CharacterItem::getNumber() const
{
    /*  if(nullptr == m_character)
          return {};

      return m_character->number();*/
    return 0;
}
QVariant CharacterItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    return VisualItem::itemChange(change, value);
    /*  QVariant newValue= value;
      m_oldPosition= pos();
      // if(change != QGraphicsItem::ItemPositionChange || !m_ctrl->collision())
      //    return VisualItem::itemChange(change, newValue);

      QList<QGraphicsItem*> list; //= collidingItems();

      // list.clear();
      QPainterPath path;
      path.addEllipse(shape().boundingRect().center(), 10, 10);
      path.connectPath(shape().translated(value.toPointF() - pos()));

      QGraphicsScene* currentScene= scene();
      auto mappedPath= mapToScene(path);
      auto collisionAtNewPosition= currentScene->items(mappedPath);
      list.append(collisionAtNewPosition);

      for(QGraphicsItem* item : list)
      {
          VisualItem* vItem= dynamic_cast<VisualItem*>(item);
          if((nullptr != vItem) && (vItem != this))
          {
              // if((vItem->getLayer() == Core::Layer::OBJECT))
              {
                  newValue= m_oldPosition;
              }
          }
      }
      QVariant var= VisualItem::itemChange(change, newValue);
      // m_newPosition= value.toPointF();
      if(newValue != m_oldPosition)
      {
          emit positionChanged();
      }
      return var;*/
}
int CharacterItem::getChildPointCount() const
{
    return m_children.size();
}
void CharacterItem::setGeometryPoint(qreal pointId, QPointF& pos)
{
    /* QRectF rect; //= m_rect
     if(m_protectGeometryChange || m_holdSize)
         return;

     switch(static_cast<int>(pointId))
     {
     case 0:
         rect.setTopLeft(pos);
         break;
     case 1:
         pos.setY(rect.topRight().x() - pos.x());
         rect.setTopRight(pos);
         break;
     case 2:
         rect.setBottomRight(pos);
         break;
     case 3:
         pos.setX(rect.bottomLeft().y() - pos.y());
         rect.setBottomLeft(pos);
         break;
     case DIRECTION_RADIUS_HANDLE:
         / *   if(pos.x() - (m_rect.width() / 2) < 0)
            {
                pos.setX(m_rect.width() / 2);
            }
            m_vision->setRadius(pos.x() - (getRadius() * 2) + m_child->at(4)->boundingRect().width() + m_rect.width() /
            2); visionChanged();* /
         break;
     case ANGLE_HANDLE:
     {
         if(!qFuzzyCompare(pos.x(), ((m_vision->getRadius() + getRadius()) / 2)))
         {
             pos.setX((m_vision->getRadius() + getRadius()) / 2);
         }
         if(pos.y() < -360)
         {
             pos.setY(-360);
         }

         if(pos.y() > 0)
         {
             pos.setY(0);
         }
         qreal angle= 360 * (std::fabs(pos.y()) / 360);
         m_vision->setAngle(angle);
         visionChanged();
     }
     break;
     default:
         // emit geometryChangeOnUnkownChild(pointId,pos);
         break;
     }
     if(rect.width() < MINI_VALUE)
     {
         rect.setWidth(MINI_VALUE);
     }
     if(rect.height() < MINI_VALUE)
     {
         rect.setHeight(MINI_VALUE);
     }
     m_diameter= qMin(rect.width(), rect.height());
     sizeChanged(m_diameter);
     switch(static_cast<int>(pointId))
     {
         / *   case 0:
                pos= m_rect.topLeft();
                m_child->value(1)->setPos(m_rect.topRight());
                m_child->value(2)->setPos(m_rect.bottomRight());
                m_child->value(3)->setPos(m_rect.bottomLeft());
                m_child->value(4)->setPos(m_vision->getRadius(),
                                          m_rect.height() / 2 - m_child->value(4)->boundingRect().height() / 2);
                // m_vision->setRadius(pos.x()-(getRadius()*2)+m_child->at(4)->boundingRect().width()+m_rect.width()/2);
                break;
            case 1:
                pos= m_rect.topRight();
                m_child->value(0)->setPos(m_rect.topLeft());
                m_child->value(2)->setPos(m_rect.bottomRight());
                m_child->value(3)->setPos(m_rect.bottomLeft());
                m_child->value(4)->setPos(m_vision->getRadius(),
                                          m_rect.height() / 2 - m_child->value(4)->boundingRect().height() / 2);
                // m_vision->setRadius(pos.x()-(getRadius()*2)+m_child->at(4)->boundingRect().width());
                break;
            case 2:
                pos= m_rect.bottomRight();
                m_child->value(0)->setPos(m_rect.topLeft());
                m_child->value(1)->setPos(m_rect.topRight());
                m_child->value(3)->setPos(m_rect.bottomLeft());
                m_child->value(4)->setPos(m_vision->getRadius(),
                                          m_rect.height() / 2 - m_child->value(4)->boundingRect().height() / 2);
                // m_vision->setRadius(pos.x()-(getRadius()*2)+m_child->at(4)->boundingRect().width());
                break;
            case 3:
                pos= m_rect.bottomLeft();
                m_child->value(0)->setPos(m_rect.topLeft());
                m_child->value(1)->setPos(m_rect.topRight());
                m_child->value(2)->setPos(m_rect.bottomRight());
                m_child->value(4)->setPos(m_vision->getRadius(),
                                          m_rect.height() / 2 - m_child->value(4)->boundingRect().height() / 2);
                // m_vision->setRadius(pos.x()-(getRadius()*2)+m_child->at(4)->boundingRect().width());
                break;
            case DIRECTION_RADIUS_HANDLE:
                m_child->value(ANGLE_HANDLE)->setPos((m_vision->getRadius() + getRadius()) / 2, -m_vision->getAngle());
                break;
            case ANGLE_HANDLE:
                break;
            default:
                break;* /
     }

     // setTransformOriginPoint(m_rect.center());*/
}
void CharacterItem::initChildPointItem()
{
    /* if(nullptr != m_child)
     {
         qDeleteAll(m_child->begin(), m_child->end());
         delete m_child;
     }
     m_child= new QVector<ChildPointItem*>();*/

    for(int i= 0; i < MAX_CORNER_ITEM; ++i)
    {
        /*ChildPointItem* tmp= new ChildPointItem(m_ctrl, i, this, (i == DIRECTION_RADIUS_HANDLE));
        tmp->setMotion(ChildPointItem::ALL);
        tmp->setRotationEnable(true);
        m_child->append(tmp);*/
    }

    /*  m_child->at(DIRECTION_RADIUS_HANDLE)->setMotion(ChildPointItem::X_AXIS);
      m_child->at(DIRECTION_RADIUS_HANDLE)->setRotationEnable(false);
      m_child->at(DIRECTION_RADIUS_HANDLE)->setVisible(false);

      m_child->at(ANGLE_HANDLE)->setMotion(ChildPointItem::Y_AXIS);
      m_child->at(ANGLE_HANDLE)->setRotationEnable(false);
      m_child->at(ANGLE_HANDLE)->setVisible(false);
      updateChildPosition();*/
}

void CharacterItem::initChildPointItemMotion()
{
    // int i= 0;
    /*  for(auto& itemChild : *m_child)
       {
           itemChild->setEditableItem(true);
           switch(i)
           {
           case DIRECTION_RADIUS_HANDLE:
               itemChild->setMotion(ChildPointItem::X_AXIS);
               break;
           case ANGLE_HANDLE:
               itemChild->setMotion(ChildPointItem::Y_AXIS);
               break;
           default:
               itemChild->setMotion(ChildPointItem::ALL);
               break;
           }
           itemChild->setRotationEnable(true);
       }*/
}

ChildPointItem* CharacterItem::getRadiusChildWidget() const
{
    /*  if(m_child->size() >= 5)
      {
          return m_child->value(DIRECTION_RADIUS_HANDLE);
      }*/
    return nullptr;
}

void CharacterItem::updateChildPosition()
{
    auto rect= m_itemCtrl->thumnailRect(); //(0, 0, m_itemCtrl->side(), m_itemCtrl->side());

    m_children.value(0)->setPos(rect.topLeft());
    m_children.value(0)->setPlacement(ChildPointItem::TopLeft);
    m_children.value(1)->setPos(rect.topRight());
    m_children.value(1)->setPlacement(ChildPointItem::TopRight);
    m_children.value(2)->setPos(rect.bottomRight());
    m_children.value(2)->setPlacement(ChildPointItem::ButtomRight);
    m_children.value(3)->setPos(rect.bottomLeft());
    m_children.value(3)->setPlacement(ChildPointItem::ButtomLeft);

    // setTransformOriginPoint(rect.center());
    if(m_itemCtrl->playableCharacter())
    {
        auto vision= m_itemCtrl->vision();
        m_children.value(DIRECTION_RADIUS_HANDLE)
            ->setPos(vision->radius() + m_itemCtrl->radius(),
                     m_itemCtrl->thumnailRect().height() / 2
                         - m_children[DIRECTION_RADIUS_HANDLE]->boundingRect().height() / 2);

        m_children[ANGLE_HANDLE]->setPos((vision->radius() + m_itemCtrl->radius()) / 2, -vision->angle());
        m_children[ANGLE_HANDLE]->setVisionHandler(true);
    }
    else
    {
        m_children[DIRECTION_RADIUS_HANDLE]->setVisible(false);
        m_children[ANGLE_HANDLE]->setVisible(false);
    }
    update();
}
void CharacterItem::addActionContextMenu(QMenu& menu)
{
    /* QMenu* stateMenu= menu.addMenu(tr("Change State"));
     QList<CharacterState*>* listOfState= Character::getCharacterStateList();
     for(auto& state : *listOfState)
     {
         QAction* act
             = stateMenu->addAction(QIcon(*state->getPixmap()), state->getLabel(), this, SLOT(characterStateChange()));
         act->setData(listOfState->indexOf(state));
     }

     QMenu* user= menu.addMenu(tr("Transform into"));
     / *for(auto& character : PlayerModel::instance()->getCharacterList())
     {
         QAction* act= user->addAction(character->name());
         act->setData(character->getUuid());

         connect(act, &QAction::triggered, this, &CharacterItem::changeCharacter);
     }* /
     QMenu* shape= menu.addMenu(tr("Vision Shape"));
     shape->addAction(m_visionShapeDisk);
     shape->addAction(m_visionShapeAngle);

     if(CharacterVision::DISK == m_vision->getShape())
     {
         m_visionShapeDisk->setChecked(true);
         m_visionShapeAngle->setChecked(false);
     }
     else
     {
         m_visionShapeDisk->setChecked(false);
         m_visionShapeAngle->setChecked(true);
     }
     //  if(m_ctrl->localGM() && nullptr != m_character)
     {
         // Actions
         auto actionlist= m_character->getActionList();
         QMenu* actions= menu.addMenu(tr("Actions"));
         auto cmd= m_character->getInitCommand();
         auto act= actions->addAction(tr("Initiative"));
         act->setData(cmd);
         connect(act, &QAction::triggered, this, &CharacterItem::runInit);

         act= actions->addAction(tr("Clean Initiative"));
         connect(act, &QAction::triggered, this, &CharacterItem::cleanInit);

         if(!actionlist.isEmpty())
         {
             for(auto& charAction : actionlist)
             {
                 auto act= actions->addAction(charAction->name());
                 act->setData(charAction->command());
                 connect(act, &QAction::triggered, this, &CharacterItem::runCommand);
             }
         }

         // Shapes
         auto shapeList= m_character->getShapeList();
         if(!shapeList.isEmpty())
         {
             QMenu* actions= menu.addMenu(tr("Shapes"));
             int i= 0;
             for(auto& charShape : shapeList)
             {
                 auto act= actions->addAction(charShape->name());
                 act->setData(i);
                 connect(act, &QAction::triggered, this, &CharacterItem::setShape);
                 ++i;
             }
             auto action= actions->addAction(tr("Clean Shape"));
             connect(action, &QAction::triggered, this, [=]() {
                 if(nullptr == m_character)
                     return;
                 m_character->setCurrentShape(nullptr);
                 update();
             });
         }
     }*/
}

void CharacterItem::runInit()
{
    /*    if(nullptr == m_character)
            return;

        auto cmd= m_character->getInitCommand();

        updateListAlias(m_diceParser.getAliases());
        if(m_diceParser.parseLine(cmd))
        {
            m_diceParser.start();
            if(!m_diceParser.getErrorMap().isEmpty())
                qWarning() << m_diceParser.humanReadableError();
            auto result= m_diceParser.getLastIntegerResults();
            int sum= std::accumulate(result.begin(), result.end(), 0);
            m_character->setInitiativeScore(sum);
            update();
        }*/
}
void CharacterItem::cleanInit()
{
    /*    if(nullptr == m_character)
            return;
        m_character->setHasInitiative(false);
        update();*/
}
void CharacterItem::runCommand()
{
    /*    if(nullptr == m_character)
            return;
        auto act= qobject_cast<QAction*>(sender());
        auto cmd= act->data().toString();

        emit runDiceCommand(cmd, m_character->getUuid());*/
}

void CharacterItem::setShape()
{
    /*    if(nullptr == m_character)
            return;
        auto act= qobject_cast<QAction*>(sender());
        auto index= act->data().toInt();

        m_character->setCurrentShape(index);
        update();*/
}

void CharacterItem::changeCharacter()
{
    /*    QAction* act= qobject_cast<QAction*>(sender());
        QString uuid= act->data().toString();*/

    /*Character* tmp= PlayerModel::instance()->getCharacter(uuid);

    Character* old= m_character;
    if(nullptr != tmp)
    {
        setCharacter(tmp);
        generatedThumbnail();
        emit ownerChanged(old, this);
        emit itemGeometryChanged(this);
    }*/
}

void CharacterItem::createActions()
{
    /*auto effect= new QGraphicsDropShadowEffect();
    effect->setOffset(2., 2.);
    effect->setColor(QColor(0, 0, 0, 191));
    setGraphicsEffect(effect);*/
    /*   updateListAlias(m_diceParser.getAliases());
       m_vision.reset(new CharacterVision(this));

       m_visionShapeDisk= new QAction(tr("Disk"), this);
       m_visionShapeDisk->setCheckable(true);
       m_visionShapeAngle= new QAction(tr("Conical"), this);
       m_visionShapeAngle->setCheckable(true);

       connect(m_visionShapeAngle, SIGNAL(triggered()), this, SLOT(changeVisionShape()));
       connect(m_visionShapeDisk, SIGNAL(triggered()), this, SLOT(changeVisionShape()));

       m_reduceLife= new QAction(tr("Reduce Life"), this);
       m_increaseLife= new QAction(tr("Increase Life"), this);

       connect(m_reduceLife, &QAction::triggered, this, [this]() {
           if(nullptr == m_character)
               return;
           auto i= m_character->getHealthPointsCurrent();
           m_character->setHealthPointsCurrent(i);
       });

       connect(m_increaseLife, &QAction::triggered, this, [this]() {
           if(nullptr == m_character)
               return;
           auto i= m_character->getHealthPointsCurrent();
           m_character->setHealthPointsCurrent(i);
       });*/
}
void CharacterItem::changeVisionShape()
{
    /*    QAction* act= qobject_cast<QAction*>(sender());
        if(act == m_visionShapeDisk)
        {
            m_visionShapeAngle->setChecked(false);
            m_visionShapeDisk->setChecked(true);
            m_vision->setShape(CharacterVision::DISK);
        }
        else if(act == m_visionShapeAngle)
        {
            m_visionShapeAngle->setChecked(true);
            m_visionShapeDisk->setChecked(false);
            m_vision->setShape(CharacterVision::ANGLE);
        }*/
}

void CharacterItem::characterStateChange()
{
    /*    QAction* act= qobject_cast<QAction*>(sender());
        if(nullptr == act)
            return;

        if(nullptr == m_character)
            return;

        auto id= act->data().toString();
        m_character->setStateId(id);

        NetworkMessageWriter* msg= new NetworkMessageWriter(NetMsg::VMapCategory, NetMsg::CharacterStateChanged);
        msg->string8(getMapId());
        msg->string8(m_id);
        msg->string8(m_character->getUuid());
        msg->string16(id);
        msg->sendToServer();*/
}

void CharacterItem::updateCharacter()
{
    /*    if(nullptr == m_character)
            return;

        NetworkMessageWriter* msg= new NetworkMessageWriter(NetMsg::VMapCategory, NetMsg::CharacterChanged);
        msg->string8(getMapId());
        msg->string8(m_id);
        msg->string8(m_character->getUuid());
        m_character->fill(*msg, true);
        msg->sendToServer();*/
}

VisualItem* CharacterItem::getItemCopy()
{
    return {};
    /*CharacterItem* charactItem= new CharacterItem(m_character, pos(), m_diameter);
    charactItem->setPos(pos());
    return charactItem;*/
}

QString CharacterItem::getParentId() const
{
    /*   if(nullptr != m_character)
        {
            Person* pers= m_character->parentPerson();
            if(nullptr != pers)
            {
                return pers->getUuid();
            }
        }*/
    return QString();
}
void CharacterItem::readCharacterStateChanged(NetworkMessageReader& msg)
{
    /*   if(nullptr == m_character)
           return;

       auto stateId= msg.string16();
       m_character->setStateId(stateId);*/
    update();
}

void CharacterItem::readCharacterChanged(NetworkMessageReader& msg)
{
    /*    if(nullptr == m_character)
            return;

        m_character->read(msg);*/
    update();
}

void CharacterItem::addChildPoint(ChildPointItem* item)
{

    // item->setPointID(m_child->size());
    m_children.append(item);
}

void CharacterItem::readPositionMsg(NetworkMessageReader* msg)
{
    /*   auto z= zValue();
       VisualItem::readPositionMsg(msg);
       if(isLocal())
       {
           blockSignals(true);
           setZValue(z);
           blockSignals(false);
       }*/
    update();
}
bool CharacterItem::isLocal() const
{
    /*PlayerModel* model= PlayerModel::instance();
    if(nullptr == model)
        return false;

    return model->isLocal(m_character);*/
    return true;
}
void CharacterItem::sendVisionMsg()
{
    /*   if(hasPermissionToMove()) // getOption PermissionMode
       {
           NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::VisionChanged);
           msg.string8(m_mapId);
           msg.string16(getCharacterId());
           msg.string16(m_id);
           m_vision->fill(&msg);
           msg.sendToServer();
       }*/
}
void CharacterItem::readVisionMsg(NetworkMessageReader* msg)
{
    //    m_vision->readMessage(msg);
    update();
}

void CharacterItem::updateItemFlags()
{
    /*    VisualItem::updateItemFlags();
        if(canBeMoved())
        {

            initChildPointItemMotion();
            / *for(auto& itemChild : m_children)
            {
                itemChild->setEditableItem(true);
                itemChild->setMotion(ChildPointItem::ALL);
                itemChild->setRotationEnable(true);
            }* /

            setFlag(QGraphicsItem::ItemIsMovable, true);
            setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
            connect(this, SIGNAL(xChanged()), this, SLOT(posChange()), Qt::UniqueConnection);
            connect(this, SIGNAL(yChanged()), this, SLOT(posChange()), Qt::UniqueConnection);
            connect(this, SIGNAL(rotationChanged()), this, SLOT(rotationChange()), Qt::UniqueConnection);
        }
        else
        {
            setFlag(QGraphicsItem::ItemIsMovable, false);
            disconnect(this, SIGNAL(xChanged()), this, SLOT(posChange()));
            disconnect(this, SIGNAL(yChanged()), this, SLOT(posChange()));
            disconnect(this, SIGNAL(rotationChanged()), this, SLOT(rotationChange()));
        }*/
}

void CharacterItem::setCharacter(Character* character)
{
    /*   if(character == m_character)
           return;

       if(nullptr != m_character)
       {
           disconnect(m_character, nullptr, this, nullptr);
       }
       m_character= character;
       if(m_character)
       {
           QSpinBox box;
           connect(m_character, &Character::currentHealthPointsChanged, this, [this]() { update(); });
           connect(m_character, &Character::avatarChanged, this, [this]() { update(); });
           connect(m_character, &Character::initCommandChanged, this, [this]() { update(); });
           connect(m_character, &Character::initiativeChanged, this, [this]() { update(); });
           connect(m_character, &Character::currentHealthPointsChanged, this, [this]() { update(); });
           connect(m_character, &Character::stateIdChanged, this, [this]() { update(); });
           connect(m_character, &Character::maxHPChanged, this, [this]() { update(); });
           connect(m_character, &Character::minHPChanged, this, [this]() { update(); });
           connect(m_character, &Character::distancePerTurnChanged, this, [this]() { update(); });
           connect(m_character, &Character::avatarChanged, this, &CharacterItem::generatedThumbnail);
       }*/
}

void CharacterItem::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    /*    if((nullptr != m_character) && (event->modifiers() & Qt::AltModifier))
        {
            auto hp= m_character->getHealthPointsCurrent();
            auto delta= event->delta();
            if(delta > 0)
                ++hp;
            else
                --hp;
            m_character->setHealthPointsCurrent(hp);
            event->accept();
            update();
        }
        else
            VisualItem::wheelEvent(event);*/
}

/*void CharacterItem::setHoldSize(bool holdSize)
{
    VisualItem::setHoldSize(holdSize);

    for(auto& itemChild : m_children)
    {
        if(itemChild == nullptr)
            continue;
        if(itemChild->getPointID() < DIRECTION_RADIUS_HANDLE)
        {
            itemChild->setMotion(holdSize ? ChildPointItem::NONE : ChildPointItem::ALL);
        }
    }
}*/

bool CharacterItem::isNpc() const
{
    /*    if(nullptr != m_character)
        {
            return m_character->isNpc();
        }*/
    return false;
}

bool CharacterItem::isPlayableCharacter() const
{
    /*   if(nullptr != m_character)
       {
           return !m_character->isNpc();
       }*/
    return false;
}
void CharacterItem::setTokenFile(QString filename)
{
    /*   QFile file(filename);
       if(file.open(QIODevice::ReadOnly))
       {
           QJsonDocument doc= QJsonDocument::fromJson(file.readAll());
           QJsonObject obj= doc.object();

           m_diameter= obj["size"].toDouble();
           / *    m_rect.setHeight(m_diameter);
               m_rect.setWidth(m_diameter);* /
           if(nullptr == m_character)
           {
               auto character= new Character();
               setCharacter(character);
           }
           m_character->readTokenObj(obj);
       }*/
}

void CharacterItem::endOfGeometryChange(ChildPointItem::Change change)
{
    if(change == ChildPointItem::Resizing)
    {
        auto oldScenePos= scenePos();
        setTransformOriginPoint(m_itemCtrl->thumnailRect().center());
        auto newScenePos= scenePos();
        auto oldPos= pos();
        m_itemCtrl->setPos(QPointF(oldPos.x() + (oldScenePos.x() - newScenePos.x()),
                                   oldPos.y() + (oldScenePos.y() - newScenePos.y())));
    }
    VisualItem::endOfGeometryChange(change);
}
