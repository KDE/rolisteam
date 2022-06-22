#include "canvasfield.h"

#include "field.h"
#include <QDebug>
#include <QGraphicsSceneContextMenuEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

QHash<int, QString>
    CanvasField::m_pictureMap({{FieldController::TEXTINPUT, ":/resources/icons/Actions-edit-rename-icon.png"},
                               {FieldController::TEXTAREA, ":/resources/icons/textarea.png"},
                               {FieldController::TEXTFIELD, ":/resources/icons/Actions-edit-rename-icon.png"},
                               {FieldController::RLABEL, ""},
                               {FieldController::SELECT, ""},
                               {FieldController::CHECKBOX, ":/resources/icons/checked_checkbox.png"},
                               {FieldController::IMAGE, ":/resources/icons/photo.png"},
                               {FieldController::WEBPAGE, ":/resources/icons/webPage.svg"},
                               {FieldController::DICEBUTTON, ""}});

bool CanvasField::m_showImageField= true;

CanvasField::CanvasField(FieldController* field) : m_field(field)
{
    m_rect.setCoords(0, 0, 0, 0);
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
}
void CanvasField::setNewEnd(QPointF nend)
{
    emit widthChanged();
    emit heightChanged();
    m_rect.setBottomRight(nend);
}
FieldController* CanvasField::getField() const
{
    return m_field;
}

void CanvasField::setField(FieldController* field)
{
    m_field= field;
}
QRectF CanvasField::boundingRect() const
{
    return m_rect;
}
QPainterPath CanvasField::shape() const
{
    QPainterPath path;
    path.moveTo(0, 0);
    path.lineTo(m_rect.width(), 0);
    path.lineTo(m_rect.width(), m_rect.height());
    path.lineTo(0, m_rect.height());
    path.closeSubpath();
    return path;
}
void CanvasField::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget)

    if(nullptr == m_field)
        return;
    painter->save();

    painter->fillRect(m_rect, m_field->bgColor());

    if(m_locked)
        painter->setPen(Qt::gray);
    else
        painter->setPen(Qt::black);

    painter->drawRect(m_rect);

    if(option->state & QStyle::State_Selected)
    {
        painter->save();
        QPen pen= painter->pen();
        pen.setColor(Qt::red);
        pen.setWidth(5);
        painter->setPen(pen);
        painter->drawRect(m_rect);
        painter->restore();
    }

    int flags= 0;
    FieldController::TextAlign align= m_field->getTextAlignValue();
    if(align < 3)
    {
        flags= Qt::AlignTop;
    }
    else if(align < 6)
    {
        flags= Qt::AlignVCenter;
    }
    else
    {
        flags= Qt::AlignBottom;
    }

    if(align % 3 == 0)
    {
        flags|= Qt::AlignRight;
    }
    else if(align % 3 == 1)
    {
        flags|= Qt::AlignHCenter;
    }
    else
    {
        flags|= Qt::AlignLeft;
    }

    if(m_pix.isNull() || m_currentType != m_field->getFieldType())
    {
        QPixmap map= QPixmap(m_pictureMap[m_field->getFieldType()]);
        if(!map.isNull())
        {
            m_pix= map.scaled(32, 32);
            m_currentType= m_field->getFieldType();
        }
    }
    if((!m_pix.isNull()) && m_showImageField)
    {
        painter->drawPixmap(m_rect.center() - m_pix.rect().center(), m_pix, m_pix.rect());
    }

    painter->drawText(m_rect, flags, m_field->getId());
    painter->restore();
}
void CanvasField::setWidth(qreal w)
{
    if(qFuzzyCompare(w, m_rect.width()) || m_locked)
        return;

    m_rect.setWidth(w);
    emit widthChanged();
    update();
}
void CanvasField::setHeight(qreal h)
{
    if(qFuzzyCompare(h, m_rect.height()) || m_locked)
        return;

    m_rect.setHeight(h);
    emit heightChanged();
    update();
}

bool CanvasField::getShowImageField()
{
    return m_showImageField;
}

void CanvasField::setShowImageField(bool showImageField)
{
    m_showImageField= showImageField;
}

void CanvasField::setMenu(QMenu& menu)
{
    Q_UNUSED(menu)
}

bool CanvasField::locked() const
{
    return m_locked;
}

void CanvasField::setLocked(bool b)
{
    if(m_locked == b)
        return;
    m_locked= b;
    emit lockedChanged();

    if(m_locked)
        setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
    else
        setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
}
