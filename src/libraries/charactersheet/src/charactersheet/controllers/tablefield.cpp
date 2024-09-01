/***************************************************************************
 * Copyright (C) 2014 by Renaud Guezennec                                   *
 * https://rolisteam.org/                                                *
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
#include "charactersheet/controllers/tablefield.h"

#include <QDebug>
#include <QJsonArray>
#include <QMouseEvent>
#include <QPainter>
#include <QUuid>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(TableFieldCat, "TableField");

#include "charactersheet/controllers/fieldcontroller.h"
#include "charactersheet/charactersheetitem.h"

void copyModel(TableModel* src, TableModel* dest, TreeSheetItem* parent)
{
    QJsonObject obj;
    src->save(obj);
    dest->load(obj, parent);
}

TableFieldController::TableFieldController(bool addCount, QObject* parent) : FieldController(TreeSheetItem::TableItem, addCount, parent)
{
}

TableFieldController::TableFieldController(QPointF topleft, bool addCount, QObject* parent)
    : FieldController(TreeSheetItem::TableItem, topleft, addCount, parent)
{
    Q_UNUSED(topleft);
    m_value= QStringLiteral("value");


}
TableFieldController::~TableFieldController() {}

TableModel* TableFieldController::model() const
{
    return m_model.get();
}

void TableFieldController::removeLine(int index)
{
    if(!m_model)
        return;
    m_model->removeLine(index);
}

void TableFieldController::removeLastLine()
{
    if(!m_model)
        return;
    QModelIndex index;
    m_model->removeLine(m_model->rowCount(index) - 1);
}

void TableFieldController::addLine()
{
    if(!m_model)
        return;

    if(m_model->rowCount() == m_displayedRow)
    {
        m_model->addRow();
        setDisplayedRow(m_displayedRow+1);
    }
    else if(m_model->rowCount()> m_displayedRow)
    {
        setDisplayedRow(m_displayedRow+1);
    }
    else if(m_model->rowCount() < m_displayedRow)
    {
        m_model->addRow();
    }
    emit requestUpdate();
}

void TableFieldController::addColumn()
{
    if(!m_model)
        return;
    auto col= new FieldController(TreeSheetItem::FieldItem, true);
    connect(col, &FieldController::updateNeeded, this, &TableFieldController::updateNeeded);
    col->setParent(this);
    col->setValueFrom(TreeSheetItem::X, 0);
    col->setValueFrom(TreeSheetItem::Y, 0);
    col->setValueFrom(TreeSheetItem::WIDTH, 0);
    col->setValueFrom(TreeSheetItem::HEIGHT, 20);
    m_model->addColumn(col);
    emit requestUpdate();
}

void TableFieldController::removeColumn(int index)
{
    if(!m_model)
        return;
    m_model->removeColumn(index);
}

void TableFieldController::appendChild(TreeSheetItem*)
{
    if(!m_model)
        return;
    m_model->addRow();
}

void TableFieldController::init()
{
    m_id= QStringLiteral("id_%1").arg(m_count);
    m_fieldType= FieldController::TABLE;
    m_model= std::make_unique<TableModel>();
    connect(m_model.get(), &TableModel::columnsInserted, this, [this](const QModelIndex& index, int first, int last){
        Q_UNUSED(index);
        Q_UNUSED(last);
        qDebug() << "columnCount Changed" << first;
        emit columnCountChanged(true, first);
    });
    connect(m_model.get(), &TableModel::columnsRemoved, this, [this](const QModelIndex& index, int first, int last){
        Q_UNUSED(index);
        Q_UNUSED(last);
        qDebug() << "columnCount Changed" << first;
        emit columnCountChanged(false, first);
    });
    addColumn();

    m_border= NONE;
    m_textAlign= FieldController::TopLeft;
    m_bgColor= Qt::transparent;
    m_textColor= Qt::black;
    m_font= font();
}

void TableFieldController::updateColumnSize()
{
    auto cols = m_model->columns();
    if(cols.size()==1)
    {
        cols[0]->setWidth(width());
    }
}

void TableFieldController::setDisplayedRow(int rCount)
{
    if(rCount == m_displayedRow)
        return;
    m_displayedRow = rCount;
    emit rowCountChanged();
}

TableFieldController::ControlPosition TableFieldController::position() const
{
    return m_position;
}

void TableFieldController::setPosition(const ControlPosition& position)
{
    m_position= position;
}

QVariant TableFieldController::valueFrom(TreeSheetItem::ColumnId col, int role) const
{
    /*if(col == TreeSheetItem::VALUE)
    {
        return m_model->childrenCount();
    }*/
    return FieldController::valueFrom(col, role);
}

bool TableFieldController::hasChildren()
{
    return columnCount() > 0;
}

int TableFieldController::childrenCount() const
{
    return columnCount();
}

TreeSheetItem* TableFieldController::childFromId(const QString& id) const
{
    return nullptr; //m_model->getFieldById(id);
}

TreeSheetItem* TableFieldController::childAt(int index) const
{
    auto cols = m_model->columns();
    if(index < 0 || index > cols.size() || cols.isEmpty())
        return nullptr;
    else
        return cols[index];
}

void TableFieldController::save(QJsonObject& json, bool exp)
{
    if(!m_model)
        return;
    if(exp)
    {
        json["type"]= "TableField";
        json["id"]= m_id;
        json["label"]= m_label;
        json["value"]= m_value;
        json["typefield"]= m_fieldType;
        QJsonObject data;
        m_model->save(data);
        json["data"]= data;
        return;
    }
    json["type"]= "TableField";
    json["id"]= m_id;
    json["typefield"]= m_fieldType;
    json["label"]= m_label;
    json["value"]= m_value;
    json["border"]= m_border;
    json["page"]= m_page;
    json["formula"]= m_formula;
    json["tooltip"]= m_tooltip;
    json["generatedCode"]= m_generatedCode;
    json["displayedRow"]=m_displayedRow;

    json["clippedText"]= m_fitFont;

    QJsonObject bgcolor;
    bgcolor["r"]= QJsonValue(m_bgColor.red());
    bgcolor["g"]= m_bgColor.green();
    bgcolor["b"]= m_bgColor.blue();
    bgcolor["a"]= m_bgColor.alpha();
    json["bgcolor"]= bgcolor;

    json["positionControl"]= m_position;

    QJsonObject textcolor;
    textcolor["r"]= m_textColor.red();
    textcolor["g"]= m_textColor.green();
    textcolor["b"]= m_textColor.blue();
    textcolor["a"]= m_textColor.alpha();
    json["textcolor"]= textcolor;

    json["font"]= m_font.toString();
    json["textalign"]= m_textAlign;
    json["x"]= valueFrom(TreeSheetItem::X, Qt::DisplayRole).toDouble();
    json["y"]= valueFrom(TreeSheetItem::Y, Qt::DisplayRole).toDouble();
    json["width"]= valueFrom(TreeSheetItem::WIDTH, Qt::DisplayRole).toDouble();
    json["height"]= valueFrom(TreeSheetItem::HEIGHT, Qt::DisplayRole).toDouble();
    QJsonArray valuesArray;
    valuesArray= QJsonArray::fromStringList(m_availableValue);
    json["values"]= valuesArray;

    QJsonObject data;
    m_model->save(data);
    json["data"]= data;

    /*QJsonObject obj;
    m_tableCanvasField->save(obj);
    json["canvas"]= obj;*/
}

void TableFieldController::load(const QJsonObject& json)
{
    if(!m_model)
        return;
    // TODO dupplicate from Field
    m_id= json["id"].toString();
    m_border= static_cast<BorderLine>(json["border"].toInt());
    m_value= json["value"].toString();
    m_label= json["label"].toString();
    m_tooltip= json["tooltip"].toString();
    m_displayedRow= json["displayedRow"].toInt();

    m_fieldType= static_cast<FieldController::TypeField>(json["typefield"].toInt());
    m_fitFont= json["clippedText"].toBool();

    m_formula= json["formula"].toString();

    QJsonObject bgcolor= json["bgcolor"].toObject();
    int r, g, b, a;
    r= bgcolor["r"].toInt();
    g= bgcolor["g"].toInt();
    b= bgcolor["b"].toInt();
    a= bgcolor["a"].toInt();

    m_bgColor= QColor(r, g, b, a);

    QJsonObject textcolor= json["textcolor"].toObject();

    r= textcolor["r"].toInt();
    g= textcolor["g"].toInt();
    b= textcolor["b"].toInt();
    a= textcolor["a"].toInt();

    m_textColor= QColor(r, g, b, a);

    m_position= static_cast<ControlPosition>(json["positionControl"].toInt());

    m_font.fromString(json["font"].toString());

    m_textAlign= static_cast<FieldController::TextAlign>(json["textalign"].toInt());
    qreal x, y, w, h;
    x= json["x"].toDouble();
    y= json["y"].toDouble();
    w= json["width"].toDouble();
    h= json["height"].toDouble();
    m_page= json["page"].toInt();
    m_generatedCode= json["generatedCode"].toString();

    QJsonArray valuesArray= json["values"].toArray();
    auto const& list= valuesArray.toVariantList();
    for(auto& value : list)
    {
        m_availableValue << value.toString();
    }
    setX(x);
    setY(y);
    setWidth(w);
    setHeight(h);
    auto data= json["data"].toObject();

    m_model->load(data, this);
}

void TableFieldController::copyField(TreeSheetItem* oldItem, bool copyData, bool sameId)
{
    Q_UNUSED(copyData);
    auto const oldField= dynamic_cast<TableFieldController*>(oldItem);
    if(nullptr != oldField)
    {
        if(sameId)
        {
            setId(oldField->id());
        }
        setFieldType(oldField->fieldType());
        // setRect(oldField->getRect());
        setBorder(oldField->border());
        setFont(oldField->font());
        setBgColor(oldField->bgColor());
        setTextColor(oldField->textColor());
        setLabel(oldField->label());
        setFormula(oldField->formula());
        copyModel(oldField->model(), m_model.get(), this);
        setOrig(oldField);
    }
}

bool TableFieldController::mayHaveChildren() const
{
    return true;
}

void TableFieldController::loadDataItem(const QJsonObject& json)
{
    if(!m_model)
        return;
    m_id= json["id"].toString();
    setValue(json["value"].toString(), true);
    setLabel(json["label"].toString());
    setFormula(json["formula"].toString());
    setReadOnly(json["readonly"].toBool());
    m_fieldType= static_cast<FieldController::TypeField>(json["typefield"].toInt());

    QJsonArray childArray= json["children"].toArray();
    m_model->loadDataItem(childArray, this);
}

void TableFieldController::setChildFieldData(const QJsonObject& json)
{
    if(!m_model)
        return;
    m_model->setChildFieldData(json);
}

void TableFieldController::setFieldInDictionnary(QHash<QString, QString>& dict) const
{
    if(!m_model)
        return;
    FieldController::setFieldInDictionnary(dict);
    m_model->setFieldInDictionnary(dict, m_id, m_label);
}

void TableFieldController::saveDataItem(QJsonObject& json)
{
    if(!m_model)
        return;
    json["type"]= "TableField";
    json["typefield"]= m_fieldType;
    json["id"]= m_id;
    json["label"]= m_label;
    json["value"]= m_value;
    json["formula"]= m_formula;
    json["readonly"]= m_readOnly;

    QJsonArray childArray;
    m_model->saveDataItem(childArray);
    json["children"]= childArray;
}
QList<int> TableFieldController::sumColumn() const
{
    return m_model ?m_model->sumColumn() : QList<int>{};
}

int TableFieldController::rowCount() const
{
    return m_model ? m_model->rowCount(): 0;
}

int TableFieldController::displayedRow() const
{
    return m_displayedRow;
}

int TableFieldController::columnCount() const
{
    return m_model ? m_model->columnCount() : 0;
}


