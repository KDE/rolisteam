/***************************************************************************
* Copyright (C) 2014 by Renaud Guezennec                                   *
* http://www.rolisteam.org/                                                *
*                                                                          *
*  This file is part of rcse                                               *
*                                                                          *
* rcse is free software; you can redistribute it and/or modify             *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
* rcse is distributed in the hope that it will be useful,                  *
* but WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
* GNU General Public License for more details.                             *
*                                                                          *
* You should have received a copy of the GNU General Public License        *
* along with this program; if not, write to the                            *
* Free Software Foundation, Inc.,                                          *
* 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                 *
***************************************************************************/
#include "field.h"
#include <QPainter>
#include <QMouseEvent>
#include <QJsonArray>
#include <QUuid>
#include <QDebug>

Field::Field(QGraphicsItem* parent)
: CSItem(parent)
{
 init();
}

Field::Field(QPointF topleft,QGraphicsItem* parent)
    : CSItem(parent)
{
    m_rect.setTopLeft(topleft);
    m_rect.setBottomRight(topleft);
    m_value = QStringLiteral("value");
    init();

}
void Field::init()
{
    m_id = QStringLiteral("id_%1").arg(m_count);
    m_clippedText = false;
    setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsGeometryChanges|QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsFocusable);

    m_textAlign = ALignLEFT;
    m_bgColor = Qt::transparent;
    m_textColor = Qt::black;
    m_font = font();
    connect(this,&Field::xChanged,[=](){
        emit updateNeeded(this);
    });
    connect(this,&Field::yChanged,[=](){
        emit updateNeeded(this);
    });
}

QRectF Field::boundingRect() const
{
    return m_rect;
}

QVariant Field::getValueFrom(CharacterSheetItem::ColumnId id) const
{
    switch(id)
    {
    case ID:
        return m_id;
    case LABEL:
        return m_label;
    case VALUE:
        return m_value;
    case X:
        return m_rect.x();
    case Y:
        return m_rect.y();
    case WIDTH:
        return m_rect.width();
    case HEIGHT:
        return m_rect.height();
    case BORDER:
        return (int)m_border;
    case TEXT_ALIGN:
        return m_textAlign;
    case BGCOLOR:
        return m_bgColor.name(QColor::HexArgb);
    case TEXTCOLOR:
        return m_textColor.name(QColor::HexArgb);
    case VALUES:
        return m_availableValue.join(',');
    case TYPE:
        return m_currentType;
    case CLIPPED:
        return m_clippedText;
    }
    return QVariant();
}

void Field::setValueFrom(CharacterSheetItem::ColumnId id, QVariant var)
{
    switch(id)
    {
    case ID:
        setId(var.toString());
        break;
    case LABEL:
        setLabel(var.toString());
        break;
    case VALUE:
        setValue(var.toString());
        break;
    case X:
        m_rect.setX(var.toReal());
        break;
    case Y:
        m_rect.setY(var.toReal());
        break;
    case WIDTH:
        m_rect.setWidth(var.toReal());
        break;
    case HEIGHT:
        m_rect.setHeight(var.toReal());
        break;
    case BORDER:
        m_border = (BorderLine)var.toInt();
        break;
    case TEXT_ALIGN:
        m_textAlign= (TextAlign)var.toInt();
        break;
    case BGCOLOR:
        m_bgColor= var.value<QColor>();
        break;
    case TEXTCOLOR:
        m_textColor= var.value<QColor>();
        break;
    case VALUES:
        m_availableValue = var.toString().split(',');
        break;
    case TYPE:
        m_currentType= (Field::TypeField)var.toInt();
        break;
    case CLIPPED:
        m_clippedText=var.toBool();
        break;
    }
    update();
}
void Field::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    painter->save();
    painter->drawRect(m_rect);
    painter->drawText(m_rect,m_id);
    painter->restore();
}

void Field::drawField()
{

}

Field::BorderLine Field::border() const
{
    return m_border;
}

void Field::setBorder(const Field::BorderLine &border)
{
    m_border = border;
    drawField();
}

QFont Field::font() const
{
    return m_font;
}

void Field::setFont(const QFont &font)
{
    m_font = font;
    drawField();
}

void Field::mousePressEvent(QMouseEvent* ev)
{
    if(ev->button() == Qt::LeftButton)
    {
       // emit clickOn(this);
    }
}

bool Field::getClippedText() const
{
    return m_clippedText;
}

void Field::setClippedText(bool clippedText)
{
    m_clippedText = clippedText;
}

Field::TypeField Field::getCurrentType() const
{
    return m_currentType;
}

void Field::setCurrentType(const Field::TypeField &currentType)
{
    m_currentType = currentType;
}
CharacterSheetItem* Field::getChildAt(QString key) const
{
    return NULL;
}
QStringList Field::getAvailableValue() const
{
    return m_availableValue;
}

void Field::setAvailableValue(const QStringList &availableValue)
{
    m_availableValue = availableValue;
}
CharacterSheetItem::CharacterSheetItemType Field::getItemType() const
{
    return CharacterSheetItem::FieldItem;
}

void Field::save(QJsonObject& json,bool exp)
{
    if(exp)
    {
        json["type"]="field";
        json["id"]=m_id;
        json["label"]=m_label;
        json["value"]=m_value;
        return;
    }
    json["type"]="field";
    json["id"]=m_id;
    json["typefield"]=m_currentType;
    json["label"]=m_label;
    json["value"]=m_value;
    json["border"]=m_border;
    json["page"]=m_page;

    json["clippedText"]=m_clippedText;

    QJsonObject bgcolor;
    bgcolor["r"]=QJsonValue(m_bgColor.red());
    bgcolor["g"]=m_bgColor.green();
    bgcolor["b"]=m_bgColor.blue();
    bgcolor["a"]=m_bgColor.alpha();
    json["bgcolor"]=bgcolor;

    QJsonObject textcolor;
    textcolor["r"]=m_textColor.red();
    textcolor["g"]=m_textColor.green();
    textcolor["b"]=m_textColor.blue();
    textcolor["a"]=m_textColor.alpha();
    json["textcolor"]=textcolor;

    json["font"]=m_font.toString();
    json["textalign"]=m_textAlign;
    json["x"]=m_rect.x();
    json["y"]=m_rect.y();
    json["width"]=m_rect.width();
    json["height"]=m_rect.height();
    QJsonArray valuesArray;
    valuesArray=QJsonArray::fromStringList(m_availableValue);
    json["values"]=valuesArray;
}

void Field::load(QJsonObject &json,QList<QGraphicsScene*> scene)
{
    m_id = json["id"].toString();
    m_border = (BorderLine)json["border"].toInt();
    m_value= json["value"].toString();
    m_label = json["label"].toString();

    m_currentType=(Field::TypeField)json["typefield"].toInt();
    m_clippedText=json["clippedText"].toBool();


    QJsonObject bgcolor = json["bgcolor"].toObject();
    int r,g,b,a;
    r = bgcolor["r"].toInt();
    g = bgcolor["g"].toInt();
    b = bgcolor["b"].toInt();
    a = bgcolor["a"].toInt();

    m_bgColor=QColor(r,g,b,a);

    QJsonObject textcolor = json["textcolor"].toObject();

    r = textcolor["r"].toInt();
    g = textcolor["g"].toInt();
    b = textcolor["b"].toInt();
    m_textColor=QColor(r,g,b,a);

    m_font.fromString(json["font"].toString());

    m_textAlign = (TextAlign)json["textalign"].toInt();
    qreal x,y,w,h;
    x=json["x"].toDouble();
    y=json["y"].toDouble();
    w=json["width"].toDouble();
    h=json["height"].toDouble();
    m_page=json["page"].toInt();

    QJsonArray valuesArray=json["values"].toArray();
    for(auto value : valuesArray.toVariantList())
    {
        m_availableValue << value.toString();
    }
    m_rect.setRect(x,y,w,h);

    update();
}

void Field::loadDataItem(QJsonObject &json)
{
    m_id = json["id"].toString();
    m_value= json["value"].toString();
    m_label = json["label"].toString();
    m_currentType=(Field::TypeField)json["typefield"].toInt();
}

void Field::saveDataItem(QJsonObject &json)
{
    json["type"]="field";
    json["typefield"]=m_currentType;
    json["id"]=m_id;
    json["label"]=m_label;
    json["value"]=m_value;
}
QString Field::getQMLItemName()
{
    if(!m_availableValue.isEmpty())
    {
        return "SelectField";
    }
    switch(m_currentType)
    {
    case Field::TEXTFIELD:
        return "TextFieldField";
    case Field::TEXTINPUT:
        return "TextInputField";
    case Field::TEXTAREA:
        return "TextAreaField";
    case Field::CHECKBOX:
        return "CheckBoxField";
    case Field::SELECT:
        return "SelectField";
    default:
        return "";
        break;
    }
}

void Field::generateQML(QTextStream &out,CharacterSheetItem::QMLSection sec)
{
    if(sec==CharacterSheetItem::FieldSec)
    {

        out << getQMLItemName() <<" {\n";
        if(!m_availableValue.isEmpty())
        {
            out << "    availableValues:" << QStringLiteral("[\"%1\"]").arg(m_availableValue.join("\",\""))<<"\n";
            out << "    currentIndex: availableValues.find(text)\n";
            out << "    onCurrentIndexChanged:{\n";
            out << "    if(count>0)\n";
            out << "    {\n";
            out << "    "<<m_id<<".value = currentText\n";
            out << "    }}\n";
        }
        out << "    id: _"<<m_id<< "\n";
        out << "    text: "<<m_id << ".value\n";
        out << "    x:" << m_rect.x() << "*parent.realscale"<<"\n";
        if(m_clippedText)
        {
            out << "    clippedText:true\n";
        }
        out << "    y:" << m_rect.y()<< "*parent.realscale"<<"\n";
        out << "    width:" << m_rect.width() <<"*parent.realscale"<<"\n";
        out << "    height:"<< m_rect.height()<<"*parent.realscale"<<"\n";
        out << "    color: \"" << m_bgColor.name(QColor::HexArgb)<<"\"\n";
        out << "    visible: root.page == "<< m_page << "? true : false\n";
        if(m_availableValue.isEmpty())
        {
            out << "    onTextChanged: {\n";
            out << "    "<<m_id<<".value = text}\n";
        }
        out << "}\n";
    }
}
void Field::copyField(CharacterSheetItem* newItem)
{
    Field* newField =  dynamic_cast<Field*>(newItem);
    if(NULL!=newField)
    {
        setId(newField->getId());
        qDebug() << m_id << newField->getId()<<"newfield";
        setValue(newField->value());
        setRect(newField->getRect());
        setBorder(newField->border());
        setFont(newField->font());
        setBgColor(newField->bgColor());
        setTextColor(newField->textColor());
    }
}
