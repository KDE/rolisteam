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
#include "charactersheet/controllers/fieldcontroller.h"
#include <QDebug>
#include <QJsonArray>
#include <QMouseEvent>
#include <QPainter>
#include <QUuid>

#include "charactersheet/controllers/fieldcontroller.h"

void copyModel(LineModel* src, LineModel* dest, TreeSheetItem* parent)
{
    QJsonArray array;
    src->saveDataItem(array);
    dest->loadDataItem(array, parent);
}
//////////////////////////////////////////
/// @brief LineFieldItem::createLineItem
/// @return
//////////////////////////////////////////

LineFieldItem::LineFieldItem(QObject* parent) : QObject(parent) {}

LineFieldItem::~LineFieldItem() {}

void LineFieldItem::insertField(FieldController* field)
{
    m_fields.append(field);
}

FieldController* LineFieldItem::getField(int k) const
{
    if(m_fields.size() > k)
        return m_fields.at(k);
    else
        return nullptr;
}

QList<FieldController*> LineFieldItem::getFields() const
{
    return m_fields;
}

void LineFieldItem::setFields(const QList<FieldController*>& fields)
{
    m_fields= fields;
}
int LineFieldItem::getFieldCount() const
{
    return m_fields.size();
}
FieldController* LineFieldItem::getFieldById(const QString& id)
{
    for(auto& field : m_fields)
    {
        if(field->id() == id)
        {
            return field;
        }
    }
    return nullptr;
}
FieldController* LineFieldItem::getFieldByLabel(const QString& label)
{
    for(auto& field : m_fields)
    {
        if(field->label() == label)
        {
            return field;
        }
    }
    return nullptr;
}
void LineFieldItem::save(QJsonArray& json)
{
    for(auto& field : m_fields)
    {
        QJsonObject obj;
        field->save(obj);
        json.append(obj);
    }
}
void LineFieldItem::saveDataItem(QJsonArray& json)
{
    for(auto& field : m_fields)
    {
        QJsonObject obj;
        field->saveDataItem(obj);
        json.append(obj);
    }
}
void LineFieldItem::load(QJsonArray& json, TreeSheetItem* parent)
{
    for(auto const value : json)
    {
        auto field= new FieldController(TreeSheetItem::FieldItem, true);
        field->setParent(parent);
        QJsonObject obj= value.toObject();
        field->load(obj);
        m_fields.append(field);
    }
}
void LineFieldItem::loadDataItem(QJsonArray& json, TreeSheetItem* parent)
{
    for(auto const value : json)
    {
        auto field= new FieldController(TreeSheetItem::FieldItem, true);
        field->setParent(parent);
        connect(field, &FieldController::characterSheetItemChanged, parent, &TreeSheetItem::characterSheetItemChanged);
        // connect(field, &FieldController::updateNeeded, parent, &TreeSheetItem::updateNeeded);
        QJsonObject obj= value.toObject();
        field->loadDataItem(obj);
        m_fields.append(field);
    }
}

////////////////////////////////////////
//
////////////////////////////////////////
LineModel::LineModel() {}
int LineModel::rowCount(const QModelIndex& parent) const
{
    if(!parent.isValid())
        return m_lines.size();
    return 0;
}

QVariant LineModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    auto item= m_lines.at(index.row());

    if(role == LineRole)
    {
        return QVariant::fromValue<LineFieldItem*>(item);
    }
    else
    {
        int key= role - (LineRole + 1);
        return QVariant::fromValue<FieldController*>(item->getField(key / 2));
    }
    // return QVariant();
}

QHash<int, QByteArray> LineModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[LineRole]= "line";
    int i= 1;
    auto first= m_lines.first();
    for(auto& fieldLine : first->getFields())
    {
        roles[LineRole + i]= fieldLine->id().toUtf8();
        i++;
        roles[LineRole + i]= fieldLine->label().toUtf8();
        i++;
    }
    return roles;
}

void LineModel::insertLine(LineFieldItem* line)
{
    beginInsertRows(QModelIndex(), m_lines.size(), m_lines.size());
    m_lines.append(line);
    endInsertRows();
}

void LineModel::appendLine(TableField* field)
{
    if(m_lines.isEmpty())
        return;

    auto line= m_lines.last();
    QJsonArray array;
    line->save(array);
    auto fieldLine= new LineFieldItem();
    fieldLine->loadDataItem(array, field);
    insertLine(fieldLine);
}

void LineModel::clear()
{
    beginResetModel();
    qDeleteAll(m_lines);
    m_lines.clear();
    endResetModel();
}

int LineModel::childrenCount() const
{
    if(!m_lines.isEmpty())
    {
        return m_lines.size() * getColumnCount();
    }
    return 0;
}

FieldController* LineModel::getFieldById(const QString& id)
{
    for(auto& line : m_lines)
    {
        auto item= line->getFieldById(id);
        if(nullptr != item)
            return item;
    }
    return nullptr;
}

int LineModel::getColumnCount() const
{
    if(!m_lines.isEmpty())
    {
        auto line= m_lines.first();
        return line->getFieldCount();
    }
    return -1;
}

FieldController* LineModel::getField(int line, int col)
{
    if(m_lines.size() > line)
    {
        return m_lines.at(line)->getField(col);
    }
    return nullptr;
}

void LineModel::save(QJsonArray& json)
{
    for(auto& line : m_lines)
    {
        QJsonArray lineJson;
        line->save(lineJson);
        json.append(lineJson);
    }
}

void LineModel::saveDataItem(QJsonArray& json)
{
    for(auto& line : m_lines)
    {
        QJsonArray lineJson;
        line->saveDataItem(lineJson);
        json.append(lineJson);
    }
}

void LineModel::load(const QJsonArray& json, TreeSheetItem* parent)
{
    beginResetModel();
    QJsonArray::Iterator it;
    for(auto const& array : json)
    {
        QJsonArray obj= array.toArray();
        LineFieldItem* line= new LineFieldItem();
        line->load(obj, parent);
        m_lines.append(line);
    }
    endResetModel();
}

void LineModel::loadDataItem(const QJsonArray& json, TreeSheetItem* parent)
{
    beginResetModel();
    m_lines.clear();
    for(auto const& array : json)
    {
        QJsonArray obj= array.toArray();
        LineFieldItem* line= new LineFieldItem();
        line->loadDataItem(obj, parent);
        m_lines.append(line);
    }
    endResetModel();
}

void LineModel::setChildFieldData(const QJsonObject& json)
{
    for(auto& line : m_lines)
    {
        auto field= line->getFieldById(json["id"].toString());
        if(field)
        {
            field->loadDataItem(json);
            return;
        }
    }
}

void LineModel::setFieldInDictionnary(QHash<QString, QString>& dict, const QString& id, const QString& label) const
{
    if(m_lines.isEmpty())
        return;

    auto line= m_lines.at(0);
    auto fields= line->getFields();
    int i= 1; // count as user
    for(auto field : fields)
    {
        auto sum= sumColumn(field->label());
        QString key= QStringLiteral("%1:sumcol%2").arg(id).arg(i);
        dict[key]= QString::number(sum);
        if(!label.isEmpty())
        {
            key= QStringLiteral("%1:sumcol%2").arg(label).arg(i);
            dict[key]= QString::number(sum);
        }
        ++i;
    }
}

void LineModel::removeLine(int index)
{
    if(m_lines.isEmpty())
        return;
    if(m_lines.size() <= index)
        return;
    if(index < 0)
        return;
    beginRemoveRows(QModelIndex(), index, index);
    m_lines.removeAt(index);
    endRemoveRows();
}

bool LineModel::setData(const QModelIndex& index, const QVariant& data, int role)
{
    return QAbstractListModel::setData(index, data, role);
}

int LineModel::sumColumn(const QString& name) const
{
    int sum= 0;
    for(auto line : m_lines)
    {
        auto field= line->getFieldByLabel(name);
        if(nullptr == field)
        {
            // should not happen
            field= line->getFieldById(name);
        }
        if(nullptr == field)
            continue;

        sum+= field->value().toInt();
    }
    return sum;
}
///////////////////////////////////
/// \brief TableField::TableField
/// \param addCount
/// \param parent
///////////////////////////////////
TableField::TableField(bool addCount, QObject* parent) : FieldController(TreeSheetItem::TableItem, addCount, parent)
{
    init();
}

TableField::TableField(QPointF topleft, bool addCount, QObject* parent)
    : FieldController(TreeSheetItem::TableItem, topleft, addCount, parent)
{
    Q_UNUSED(topleft);
    m_value= QStringLiteral("value");
    init();
}
TableField::~TableField() {}

LineModel* TableField::getModel() const
{
    return m_model;
}

void TableField::removeLine(int index)
{
    m_model->removeLine(index);
}

void TableField::removeLastLine()
{
    QModelIndex index;
    m_model->removeLine(m_model->rowCount(index) - 1);
}

void TableField::addLine()
{
    emit lineMustBeAdded(this);
}

void TableField::appendChild(TreeSheetItem*)
{
    m_model->appendLine(this);
}

void TableField::init()
{
    m_id= QStringLiteral("id_%1").arg(m_count);
    m_fieldType= FieldController::TABLE;
    m_model= new LineModel();

    m_border= NONE;
    m_textAlign= FieldController::TopLeft;
    m_bgColor= Qt::transparent;
    m_textColor= Qt::black;
    m_font= font();
}

TableField::ControlPosition TableField::getPosition() const
{
    return m_position;
}

void TableField::setPosition(const ControlPosition& position)
{
    m_position= position;
}

QVariant TableField::valueFrom(TreeSheetItem::ColumnId col, int role) const
{
    if(col == TreeSheetItem::VALUE)
    {
        return m_model->childrenCount();
    }
    return FieldController::valueFrom(col, role);
}

bool TableField::hasChildren()
{
    return m_model->rowCount(QModelIndex()) > 0;
}

int TableField::childrenCount() const
{
    return m_model->childrenCount();
}

int TableField::getMaxVisibleRowCount() const
{
    return 0;
}

TreeSheetItem* TableField::getRoot()
{
    return nullptr;
}

TreeSheetItem* TableField::childFromId(const QString& id) const
{
    return m_model->getFieldById(id);
}

TreeSheetItem* TableField::childAt(int index) const
{
    int itemPerLine= m_model->getColumnCount();
    int line= index / itemPerLine;
    int col= index - (line * itemPerLine);
    auto item= m_model->getField(line, col);
    return item;
}

void TableField::save(QJsonObject& json, bool exp)
{
    if(exp)
    {
        json["type"]= "TableField";
        json["id"]= m_id;
        json["label"]= m_label;
        json["value"]= m_value;
        json["typefield"]= m_fieldType;
        QJsonArray childArray;
        m_model->save(childArray);
        json["children"]= childArray;
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

    QJsonArray childArray;
    m_model->save(childArray);
    json["children"]= childArray;

    /*QJsonObject obj;
    m_tableCanvasField->save(obj);
    json["canvas"]= obj;*/
}

void TableField::load(const QJsonObject& json)
{
    // TODO dupplicate from Field
    m_id= json["id"].toString();
    m_border= static_cast<BorderLine>(json["border"].toInt());
    m_value= json["value"].toString();
    m_label= json["label"].toString();
    m_tooltip= json["tooltip"].toString();

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
    QJsonArray childArray= json["children"].toArray();

    m_model->load(childArray, this);
}

void TableField::copyField(TreeSheetItem* oldItem, bool copyData, bool sameId)
{
    Q_UNUSED(copyData);
    auto const oldField= dynamic_cast<TableField*>(oldItem);
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
        copyModel(oldField->getModel(), m_model, this);
        setOrig(oldField);
    }
}

bool TableField::mayHaveChildren() const
{
    return true;
}

int TableField::lineNumber() const
{
    if(nullptr == m_model)
        return -1;

    return m_model->rowCount(QModelIndex());
}

int TableField::itemPerLine() const
{
    if(nullptr == m_model)
        return -1;

    return m_model->getColumnCount();
}

void TableField::fillModel()
{
    m_model->clear();
    // m_tableCanvasField->fillLineModel(m_model, this);
}

void TableField::loadDataItem(const QJsonObject& json)
{
    m_id= json["id"].toString();
    setValue(json["value"].toString(), true);
    setLabel(json["label"].toString());
    setFormula(json["formula"].toString());
    setReadOnly(json["readonly"].toBool());
    m_fieldType= static_cast<FieldController::TypeField>(json["typefield"].toInt());

    QJsonArray childArray= json["children"].toArray();
    m_model->loadDataItem(childArray, this);
}

void TableField::setChildFieldData(const QJsonObject& json)
{
    m_model->setChildFieldData(json);
}

void TableField::setFieldInDictionnary(QHash<QString, QString>& dict) const
{
    FieldController::setFieldInDictionnary(dict);
    m_model->setFieldInDictionnary(dict, m_id, m_label);
}

void TableField::saveDataItem(QJsonObject& json)
{
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
int TableField::sumColumn(const QString& name) const
{
    return m_model->sumColumn(name);
}