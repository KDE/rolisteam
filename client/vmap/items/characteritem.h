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
#ifndef CHARACTERITEM_H
#define CHARACTERITEM_H

#include <QAction>
#include <QPointer>

#include "data/characterstate.h"
#include "data/charactervision.h"
#include "data/person.h"
#include "diceparser.h"
#include "visualitem.h"
#include <memory>

/**
 * @brief represents any character on map.
 */
class CharacterItem : public VisualItem
{
    Q_OBJECT
public:
    /**
     * @brief CharacterItem
     */
    CharacterItem();
    /**
     * @brief CharacterItem
     * @param m
     * @param center
     * @param diameter
     */
    CharacterItem(Character* m, const QPointF& center, qreal diameter= 40.0);
    /**
     * @brief serialisation function to write data
     */
    virtual void writeData(QDataStream& out) const override;
    /**
     * @brief serialisation function to read data.
     */
    virtual void readData(QDataStream& in) override;
    /**
     * @brief getType
     * @return
     */
    virtual VisualItem::ItemType getType() const override;

    /**
     * @brief gives the bounding rect of the ellipse
     */
    virtual QRectF boundingRect() const override;
    /**
     * @brief shape
     * @return
     */
    virtual QPainterPath shape() const override;
    /**
     * @brief modifies the ellipse size and shape.
     */
    virtual void setNewEnd(QPointF& nend) override;
    /**
     * @brief paint the ellipse at the correct position
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget= nullptr) override;

    /**
     * @brief fillMessage
     * @param msg
     */
    virtual void fillMessage(NetworkMessageWriter* msg) override;
    /**
     * @brief readItem
     * @param msg
     */
    virtual void readItem(NetworkMessageReader* msg) override;
    /**
     * @brief setGeometryPoint
     * @param pointId
     * @param pos
     */
    void setGeometryPoint(qreal pointId, QPointF& pos) override;
    /**
     * @brief initChildPointItem
     */
    virtual void initChildPointItem() override;
    /**
     * @brief resizeContents
     * @param rect
     * @param keepRatio
     */
    void resizeContents(const QRectF& rect, TransformType transformType= KeepRatio) override;
    /**
     * @brief updateChildPosition
     */
    void updateChildPosition() override;
    /**
     * @brief itemChange
     * @param change
     * @param value
     * @return
     */
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

    /**
     * @brief addActionContextMenu
     */
    virtual void addActionContextMenu(QMenu&) override;
    /**
     * @brief getItemCopy
     * @return
     */
    virtual VisualItem* getItemCopy() override;
    /**
     * @brief getCharacterId
     * @return
     */
    QString getCharacterId() const;
    /**
     * @brief getParentId
     * @return
     */
    QString getParentId() const;
    /**
     * @brief getCenter
     * @return
     */
    const QPointF& getCenter() const;
    /**
     * @brief getRadius
     * @return
     */
    qreal getRadius() const;
    /**
     * @brief addChildPoint
     * @param item
     */
    void addChildPoint(ChildPointItem* item);
    /**
     * @brief getChildPointCount
     * @return
     */
    int getChildPointCount() const;
    /**
     * @brief setDefaultVisionParameter
     * @param radius
     * @param angle
     */
    void setDefaultVisionParameter(CharacterVision::SHAPE, qreal radius, qreal angle);
    /**
     * @brief getVision
     * @return
     */
    CharacterVision* getVision() const;
    /**
     * @brief getRadiusChildWidget
     */
    ChildPointItem* getRadiusChildWidget();
    /**
     * @brief isLocal
     * @return
     */
    virtual bool isLocal() const override;
    /**
     * @brief isPlayableCharacter
     * @return
     */
    bool isPlayableCharacter();
    /**
     * @brief setSize
     * @param size
     */
    virtual void setSize(QSizeF size) override;
    /**
     * @brief setCharacterIsMovable if isMovable is true and the character is local then the item is mavable
     */
    void updateItemFlags() override;
    /**
     * @brief isNpc
     * @return
     */
    bool isNpc();
    /**
     * @brief setRectSize
     * @param x
     * @param y
     * @param w
     * @param h
     */
    virtual void setRectSize(qreal x, qreal y, qreal w, qreal h) override;
    /**
     * @brief readCharacterStateChanged
     * @param msg
     */
    void readCharacterStateChanged(NetworkMessageReader& msg);
    /**
     * @brief getColor
     * @return
     */
    virtual QColor getColor() override;
    /**
     * @brief readVisionMsg
     * @param msg
     */
    void readVisionMsg(NetworkMessageReader* msg);
    /**
     * @brief sendVisionMsg
     */
    void sendVisionMsg();
    /**
     * @brief setChildrenVisible
     * @param b
     */
    void setChildrenVisible(bool b) override;
    void updateCharacter();
    void readCharacterChanged(NetworkMessageReader& msg);
    void setCharacter(Character* character);
    Character* getCharacter() const;
    void setTokenFile(QString);
    void setNumber(int);
    QString getName() const;
    int getNumber() const;
    void setHoldSize(bool holdSize) override;
    const QPainterPath getTokenShape() const;
    const QRectF& getTextRect() const;
signals:
    /**
     * @brief positionChanged
     */
    void positionChanged();
    /**
     * @brief geometryChangeOnUnkownChild
     * @param pointId
     * @param F
     */
    void geometryChangeOnUnkownChild(qreal pointId, QPointF& F);

    /**
     * @brief localItemZValueChange
     */
    void localItemZValueChange(CharacterItem*);
    /**
     * @brief ownerChanged
     * @param old
     */
    void ownerChanged(Character* old, CharacterItem*);

    void runDiceCommand(QString cmd, QString uuid);

public slots:
    /**
     * @brief characterHasBeenDeleted
     */
    void characterHasBeenDeleted(Character*);
    /**
     * @brief changeVisionShape
     */
    void changeVisionShape();
    /**
     * @brief sizeChanged
     * @param m_size
     */
    void sizeChanged(qreal m_size);
    /**
     * @brief readPositionMsg
     * @param msg
     */
    virtual void readPositionMsg(NetworkMessageReader* msg) override;
    /**
     * @brief endOfGeometryChange
     */
    void endOfGeometryChange() override;
    /**
     * @brief generatedThumbnail
     */
    void generatedThumbnail();
    void cleanInit();
    void runInit();

protected:
    /**
     * @brief canBeMoved
     * @return
     */
    // virtual bool canBeMoved() const;

    virtual void wheelEvent(QGraphicsSceneWheelEvent* event) override;
protected slots:
    void runCommand();
    void setShape();
private slots:
    /**
     * @brief createActions
     */
    void createActions() override;
    /**
     * @brief characterStateChange
     */
    void characterStateChange();
    /**
     * @brief changeCharacter
     */
    void changeCharacter();

private:
    /**
     * @brief getSubTitle
     * @return
     */
    QString getSubTitle() const;
    void visionChanged();
    void initChildPointItemMotion();
    bool canBeMoved() const override;

private:
    QPointer<Character> m_character;
    QPointF m_center;
    qreal m_diameter;
    QPixmap* m_thumnails;
    QRectF m_rectText;
    QString m_title;
    DiceParser m_diceParser;

    // QAction*
    QAction* m_visionShapeDisk= nullptr;
    QAction* m_visionShapeAngle= nullptr;
    QAction* m_reduceLife= nullptr;
    QAction* m_increaseLife= nullptr;

    // sight
    std::unique_ptr<CharacterVision> m_vision;
    QPointF m_oldPosition;
    // QPointF m_newPosition;

    bool m_protectGeometryChange;
    bool m_visionChanged;
};

#endif // CHARACTERITEM_H
